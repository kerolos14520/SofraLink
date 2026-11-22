# Team Workflow

| Member | Responsibility | Feature branch |
|---|---|---|
| 1 — Kerolos | Shared models, MatchingEngine, C++ API, CMake and integration | `feature/core-matching-api` |
| 2 | DonationManager | `feature/donation-management` |
| 3 | RequestManager and sample accepted-food rules | `feature/request-management` |
| 4 | Validation and SearchService | `feature/search-validation` |
| 5 | Frontend | `feature/frontend-ui` |
| 6 | JsonStorage and ReportService | `feature/storage-reports` |

## Sequence
1. Member 1 commits the shared model headers and starter build to `dev`.
2. Every member pulls the updated `dev` before creating/continuing their feature branch.
3. Each member reads `AGENTS.md` and their task PDF.
4. Each member uses AI only for their assigned module, then compiles and tests the result.
5. Each member pushes their feature branch and opens a Pull Request into `dev`.
6. Member 1 reviews interface compatibility and integrates the modules.
7. The team tests the end-to-end flow before merging a stable release into `main`.

## Vibe-coding discipline
- AI-generated code must be compiled, tested, reviewed, and understood by the member submitting it.
- Ask the AI to inspect the repository and list intended file changes before editing.
- Do not ask AI to rewrite the entire project for a single-member task.
- If an interface is missing or conflicts, stop and ask Member 1; do not invent a replacement.
