# Agent Instructions

## Build Output

- Build Creation Live in this repo's own `build/` directory.
- Do not point builds at the user's personal clones or another agent's build tree.
- Use the repo-root presets or `scripts/Build-Suite.ps1` from the umbrella repo when possible so all agents configure this app the same way.
- Set `JUCE_DIR` in the environment or pass `-DJUCE_DIR=<path-to-JUCE>` explicitly when configuring.

## Shared Build Environment

- Match Creation Engine's LLVM/vcpkg discovery pattern unless there is a concrete reason to diverge.
- Prefer the already-built shared LLVM install on this machine before asking for a new LLVM rebuild.
- **Never build, rebuild, or touch LLVM — directly or as a side effect of any `vcpkg` command (including `vcpkg install` in manifest mode, which reconciles the whole dependency list) — without an explicit, in-the-moment yes from the user.** See the root `AGENTS.md`'s LLVM / vcpkg Build Rule for the full incident this is based on. If blocked, stop and ask — never act.
- **Under no circumstances write, download, build, or install anything on the C: drive. Period.** No exceptions for size or a tool's default location — redirect to a D:-drive path instead. See the root `AGENTS.md`'s C: Drive Rule.

