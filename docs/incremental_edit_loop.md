# Incremental Edit Loop

`scripts/rift_incremental_edit.sh` automates document updates across RIFT stages.

1. The script searches for the highest `rift-N` directory and creates an alias `rift-exe` for the corresponding executable.
2. Each time you provide an edit prompt, it copies `current.riftrc.N.in` to `current.riftrc.(N+1).in`, applies the edit via `apply_edit`, and runs `rift-(N+1).exe`.
3. After a successful run the alias is updated so further edits operate on the new stage.

This lightweight helper demonstrates how RIFT artifacts can drive incremental documentation updates.
