# Creation Live Language Rollout

Creation Live will use the shared Creation language, but only through live-safe domains and intrinsics.

Planned domain gates:

- allowed: `shared`, `live`, `scene`, `broadcast`
- blocked: `gameplay`, `world`, `physics`, `instrument`, `mixer`, `timeline`, `render`

Planned live-specific layers:

1. scene activation and transition intrinsics
2. cue stack and run-of-show logic
3. media / overlay playback triggers
4. stream-state and health actions

The scaffold enforces the policy boundary now with `Language/AppLanguagePolicy.*`, even before the full compiler/runtime is wired in.

