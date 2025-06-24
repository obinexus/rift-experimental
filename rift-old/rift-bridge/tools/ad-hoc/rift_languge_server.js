// RIFT LSP server implementation
class RiftLanguageServer {
    private compiler: RiftWasmCompiler;
    private diagnosticPublisher: DiagnosticPublisher;
    
    async initialize(params: InitializeParams): Promise<InitializeResult> {
        return {
            capabilities: {
                textDocumentSync: TextDocumentSyncKind.Incremental,
                diagnosticProvider: {
                    identifier: 'rift-compiler',
                    interFileDependencies: true,
                    workspaceDiagnostics: true
                },
                completionProvider: {
                    triggerCharacters: ['.', ':', '='],
                    resolveProvider: true
                },
                semanticTokensProvider: {
                    legend: {
                        tokenTypes: ['keyword', 'type', 'function', 'variable'],
                        tokenModifiers: ['declaration', 'readonly', 'static']
                    },
                    full: { delta: true }
                }
            }
        };
    }
    
    async compileIncremental(uri: string, changes: TextDocumentContentChangeEvent[]) {
        const compilation = await this.compiler.compileStages(document.getText());
        
        // Publish diagnostics for each stage
        compilation.stages.forEach((stage, index) => {
            this.diagnosticPublisher.publishDiagnostics(uri, {
                diagnostics: stage.diagnostics,
                source: `rift-stage-${index}`,
                version: document.version
            });
        });
    }
}
