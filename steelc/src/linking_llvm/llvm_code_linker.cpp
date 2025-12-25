#include "llvm_code_linker.h"

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <optional>
#include <cstdint>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/AsmParser/Parser.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/TargetRegistry.h>

#include <codegen/code_artifact.h>
#include <codegen/codegen_config.h>
#include <linking/link_result.h>

link_result llvm_code_linker::link(const std::vector<code_artifact>& artifacts, const codegen_config& cfg) {
	if (artifacts.empty()) {
		throw std::runtime_error("No artifacts provided for linking");
	}

	initialize_llvm_once();

	// load all artifacts as LLVM modules
	// (we can also check formats here)
	std::vector<std::unique_ptr<Module>> modules;
	modules.reserve(artifacts.size());
	for (const auto& a : artifacts) {
		if (a.kind != ARTIFACT_IR || a.format != cfg.ir_format) {
			throw std::runtime_error("Unsupported artifact kind or format for LLVM linking");
		}

		modules.push_back(module_from_artifact(a));
	}

	if (modules.empty()) {
		throw std::runtime_error("No valid LLVM modules to link");
	}

	std::unique_ptr<Module> composite = std::move(modules.front());
	Linker linker(*composite);
	for (size_t i = 1; i < modules.size(); ++i) {
		if (linker.linkInModule(std::move(modules[i]))) {
			throw std::runtime_error("Failed to link LLVM module");
		}
	}

	// verify linked module
	{
		std::string err_msg;
		raw_string_ostream os(err_msg);
		if (verifyModule(*composite, &os)) {
			os.flush();
			throw std::runtime_error("Linked LLVM module is invalid: " + err_msg);
		}
	}

	Triple target_triple = Triple(cfg.target_triple.empty()
		? sys::getDefaultTargetTriple()
		: cfg.target_triple);
	std::string cpu = cfg.cpu.empty()
		? "generic"
		: cfg.cpu;
	std::string feats = ""; // currently not supported

	std::string target_error;
	const Target* target = TargetRegistry::lookupTarget(target_triple, target_error);
	if (!target) {
		throw std::runtime_error("Invalid llvm code target: " + target_error);
	}

	TargetOptions topts;

	std::optional<Reloc::Model> reloc_model = std::nullopt; // will need to change this for non COFF builds

	std::unique_ptr<TargetMachine> tm(
		target->createTargetMachine(
			target_triple,
			cpu,
			feats,
			topts,
			reloc_model
		)
	);
	if (!tm) {
		throw std::runtime_error("Failed to create target machine");
	}

	composite->setTargetTriple(target_triple);
	composite->setDataLayout(tm->createDataLayout());

	// run optimization pipeline
	{
		llvm::PassBuilder pb(tm.get());

		llvm::LoopAnalysisManager lam;
		llvm::FunctionAnalysisManager fam;
		llvm::CGSCCAnalysisManager cgsccam;
		llvm::ModuleAnalysisManager mam;

		pb.registerModuleAnalyses(mam);
		pb.registerCGSCCAnalyses(cgsccam);
		pb.registerFunctionAnalyses(fam);
		pb.registerLoopAnalyses(lam);
		pb.crossRegisterProxies(lam, fam, cgsccam, mam);

		llvm::ModulePassManager mpm;
		const OptimizationLevel ol = get_optimization_level(cfg.optimization_level);
		mpm = pb.buildPerModuleDefaultPipeline(ol);

		mpm.run(*composite, mam);
	}

	// emit to file
	SmallVector<char, 0> buffer;
	raw_svector_ostream bf_stream(buffer);
	legacy::PassManager codegen_pm;
	if (tm->addPassesToEmitFile(codegen_pm, bf_stream, nullptr, CodeGenFileType::ObjectFile)) {
		throw std::runtime_error("Target machine cannot emit object files");
	}
	codegen_pm.run(*composite);

	link_result result;

	// prepare output artifact
	code_artifact linked_object{
		.kind = ARTIFACT_OBJECT,
		.src_relpath = "", // n/a
		.name = "linked",
		.extension = get_object_extension(target_triple.getOS()),
		.format = get_object_format(target_triple.getOS()),
		.is_binary = true,
		.bytes = std::vector<uint8_t>(buffer.begin(), buffer.end()),
	};
	
	result.final_intermediate = linked_object;

	return result;
}
bool llvm_code_linker::supports(const code_artifact& artifact) {
	return false;
}

void llvm_code_linker::initialize_llvm_once() {
	static bool llvm_initialized = false;

	if (llvm_initialized) return;

	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmParsers();
	InitializeAllAsmPrinters();
	llvm_initialized = true;
}

std::unique_ptr<Module> llvm_code_linker::module_from_artifact(const code_artifact& a) {
	if (a.format == "llvm-asm") {
		if (a.is_binary) throw std::runtime_error("Expected text data for llvm-asm artifact");

		SMDiagnostic err;
		auto mod = parseAssemblyString(a.text, err, ctx);
		if (!mod) {
			std::string err_msg;
			raw_string_ostream os(err_msg);
			err.print("llvm_code_linker", os);
			os.flush();
			throw std::runtime_error("Failed to parse LLVM assembly: " + err_msg);
		}
		return mod;
	}
	else if (a.format == "llvm-bc") {
		auto buf = MemoryBuffer::getMemBuffer(
			StringRef(reinterpret_cast<const char*>(a.bytes.data()), a.bytes.size()),
			"llvm_bc_artifact",
			false
		);
		auto mod_or_err = parseBitcodeFile(buf->getMemBufferRef(), ctx);
		if (!mod_or_err) {
			std::string err_msg;
			raw_string_ostream os(err_msg);
			handleAllErrors(mod_or_err.takeError(), [&](const ErrorInfoBase &EIB) {
				EIB.log(os);
			});
			os.flush();
			throw std::runtime_error("Failed to parse LLVM bitcode: " + err_msg);
		}
		return std::move(*mod_or_err);
	}
	else {
		throw std::runtime_error("Unsupported artifact format for LLVM module: " + a.format);
	}
}
OptimizationLevel llvm_code_linker::get_optimization_level(int level) {
	switch (level) {
	case 0:
		return OptimizationLevel::O0;
	case 1:
		return OptimizationLevel::O1;
	case 2:
		return OptimizationLevel::O2;
	case 3:
		return OptimizationLevel::O3;

	default:
		throw std::runtime_error("Invalid optimization level: " + std::to_string(level));
	}
}

std::string llvm_code_linker::get_object_format(Triple::OSType os) {
	switch (os) {
	case Triple::Win32:
		return "COFF";

	case Triple::Linux:
		return "ELF";

	case Triple::MacOSX:
		return "MachO";

	default:
		throw std::runtime_error("Unsupported OS for object format");
	}
}
std::string llvm_code_linker::get_object_extension(Triple::OSType os) {
	switch (os) {
	case Triple::Win32:
		return ".obj";

	case Triple::Linux:
		return ".o";

	case Triple::MacOSX:
		return ".o";

	default:
		throw std::runtime_error("Unsupported OS for object file extension");
	}
}
