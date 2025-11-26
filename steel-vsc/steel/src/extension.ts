import * as vscode from 'vscode';
import * as path from 'path';

export function activate(context: vscode.ExtensionContext) {
	let build = vscode.commands.registerCommand('steel.build', async () => {
		const stprojFile = await findStprojFile();
		if (!stprojFile) {
			vscode.window.showErrorMessage('No .stproj file found in the workspace.');
			return;
		}

		const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
		if (!workspaceFolder) {
			vscode.window.showErrorMessage('No workspace folder open.');
			return;
		}

		const relativePath = `./${path.basename(stprojFile)}`;
		
		// Create or reuse a terminal
		const terminal = vscode.window.createTerminal('Steel Build');
		terminal.show();
		terminal.sendText(`steelc "${relativePath}"`);
	});

    let run = vscode.commands.registerCommand('steel.run', async () => {
		const stprojFile = await findStprojFile();
		if (!stprojFile) {
			vscode.window.showErrorMessage('No .stproj file found in the workspace.');
			return;
		}

		const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
		if (!workspaceFolder) {
			vscode.window.showErrorMessage('No workspace folder open.');
			return;
		}

		const relativePath = `./${path.basename(stprojFile)}`;
		const projectName = path.basename(stprojFile, '.stproj');
		const exePath = `./build/${projectName}.exe`;
		
		// Create a terminal for build and run
		const terminal = vscode.window.createTerminal('Steel Run');
		terminal.show();
		terminal.sendText(`cls ; steelc "${relativePath}" ; if ($?) { cls ; & "${exePath}" }`);
    });

	context.subscriptions.push(build);
	context.subscriptions.push(run);
}

async function findStprojFile(): Promise<string | null> {
	const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
	if (!workspaceFolder) {
		return null;
	}

	const files = await vscode.workspace.findFiles('*.stproj', '**/build/**', 1);
	if (files.length > 0) {
		return files[0].fsPath;
	}

	return null;
}

export function deactivate() {}
