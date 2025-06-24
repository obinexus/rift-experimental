// Monaco WebAssembly language support
monaco.languages.register({
    id: 'rift',
    extensions: ['.rift'],
    aliases: ['RIFT', 'rift']
});

// Real-time governance visualization
class GovernanceVisualization {
    visualizeViolations(editor: monaco.editor.IStandaloneCodeEditor, violations: GovernanceViolation[]) {
        const decorations = violations.map(violation => ({
            range: new monaco.Range(
                violation.range.start.line + 1,
                violation.range.start.character + 1,
                violation.range.end.line + 1,
                violation.range.end.character + 1
            ),
            options: {
                className: `governance-violation severity-${violation.severity}`,
                hoverMessage: {
                    value: `**${violation.policy}**: ${violation.message}`
                },
                minimap: {
                    color: this.getSeverityColor(violation.severity),
                    position: monaco.editor.MinimapPosition.Inline
                }
            }
        }));
        
        editor.deltaDecorations([], decorations);
    }
}
