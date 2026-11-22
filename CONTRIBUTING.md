# Contributing to SofraLink

## Branches
- `main`: stable milestone branch; no direct commits.
- `dev`: shared integration branch.
- Feature branches: one per assigned module.

## Assigned branches
| Member | Responsibility | Branch |
|---|---|---|
| 1 — Kerolos | Shared models, MatchingEngine, API and integration | `feature/core-matching-api` |
| 2 | DonationManager | `feature/donation-management` |
| 3 | RequestManager and sample acceptance rules | `feature/request-management` |
| 4 | Validation and SearchService | `feature/search-validation` |
| 5 | Frontend | `feature/frontend-ui` |
| 6 | JsonStorage and ReportService | `feature/storage-reports` |

## Start work
```bash
git clone https://github.com/kerolos14520/SofraLink.git
cd SofraLink
git switch dev
git pull origin dev
git switch -c YOUR_ASSIGNED_BRANCH
```
If your assigned branch already exists, switch to it instead of creating it again.

## Commit and push
```bash
git status
git add PATH/TO/YOUR/FILES
git commit -m "feat: describe your change"
git push -u origin YOUR_ASSIGNED_BRANCH
```

## Pull Request
Open a Pull Request with:
- Base: `dev`
- Compare: your feature branch

Describe what you implemented, the files changed, build/test commands and actual results, and remaining limitations. Do not merge your own PR unless Member 1 asks you to.

## Shared interface changes
Do not change shared model fields, enum values, function signatures, API routes, or JSON keys independently. Ask Member 1 first. The contract must be updated and all affected members notified before the change is implemented.

## Safety and data
Use simulated data only. Do not commit real personal information, credentials, secrets, or runtime-generated records.
