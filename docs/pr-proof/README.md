# RIFT Peer Review Proofs (`pr-proof/`) — README

This directory contains **peer review proof artifacts** for the RIFT project. All documents here serve as formal evidence of technical validation, governance compliance, and systematic review, supporting the requirements and methodology described in the main [RIFT README](../README.md).

## Purpose

- **Peer Review Proofs**: Provide verifiable documentation of formal proofs, cryptographic standards, automaton minimization, and protocol validation relevant to RIFT's compiler and governance model.
- **Governance Model Support**: Demonstrate compliance with the RIFT governance framework, as enforced by `rift-N.exe` and `rift.exe` in CI/CD pipelines.
- **Project Requirement Feedback**: Enable systematic review and feedback on project requirements, ensuring all technical and governance criteria are met before release.

## Usage in CI/CD

- **Automated Validation**: CI/CD pipelines must reference and validate these proofs as part of the build and deployment process.
- **Governance Enforcement**: Proofs are checked against governance contracts (`.riftrc.N`) to ensure all compliance gates are satisfied.
- **Documentation Integration**: Artifacts here are linked in the main documentation (`/docs/`) and referenced in technical specifications.

## Directory Contents

- `Aegis.pdf` — AEGIS methodology and formal proof of systematic compiler design.
- `Cryptographic_Standard_V1_0.pdf` — Cryptographic primitives and protocol validation.
- `Automaton State Minimization and AST Optimization.pdf` — Formal automaton and AST proofs.
- `Formal Proof of Zzero-Knowledge Protocol.pdf` — Security and privacy validation.
- Additional peer-reviewed documents supporting RIFT's technical claims.

## Review Process

1. **Submission**: New proofs are added to this directory and referenced in the main documentation.
2. **Review**: Each proof is reviewed by project stakeholders and validated in CI/CD.
3. **Feedback**: Review feedback is incorporated into project requirements and governance files.
4. **Approval**: Only validated proofs are accepted for release and compliance certification.

## References

- [Project README](../README.md)
- [Governance Validation Framework](../poc/gov/README.md)
- [RIFT Documentation](../docs/)

---

*All peer review proofs are required for compliance with the RIFT governance model and must be validated as part of the CI/CD process.*