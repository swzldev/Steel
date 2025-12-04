import * as vscode from 'vscode';
import * as path from 'path';

export function activate(context: vscode.ExtensionContext) {
	let build = vscode.commands.registerCommand('steel.build', async () => {
		const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
		if (!workspaceFolder) {
			vscode.window.showErrorMessage('No workspace folder open.');
			return;
		}
		
		// Run `steelc build` with the current workspace folder
		const terminal = vscode.window.createTerminal('Steel Build');
		terminal.show();
		terminal.sendText(`steelc build "${workspaceFolder.uri.fsPath}"`);
	});

	context.subscriptions.push(build);
}

export function deactivate() {}
