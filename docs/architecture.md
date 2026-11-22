# SofraLink — Architecture

```text
Browser (HTML / CSS / JavaScript)
             | HTTP + JSON
             v
        C++ API Server
             |
             v
       Domain Services
  DonationManager / RequestManager
  Validation / SearchService
  MatchingEngine / ReportService
             |
             v
          JsonStorage
             |
             v
       data/runtime/*.json
```

## Module ownership
- Member 1: shared models/enums, MatchingEngine, ApiServer, CMake integration and end-to-end integration.
- Member 2: DonationManager.
- Member 3: RequestManager and sample `accepted-food-rules.md` in coordination with Member 4.
- Member 4: Validation and SearchService.
- Member 5: HTML/CSS/JavaScript frontend.
- Member 6: JsonStorage and ReportService.

## Data structures
- `std::vector`: collections of donations, requests and match results.
- `std::optional`: lookup results that may not exist.
- `std::priority_queue` or sorting where appropriate for expiry/urgency ordering.
- `std::unordered_map` may be used internally for efficient ID lookup if it remains compatible with the public interfaces.

## Data flow
1. Frontend submits JSON to C++ API.
2. API parses the body and calls Validation.
3. API calls DonationManager or RequestManager.
4. After a successful state change, API coordinates persistence through JsonStorage.
5. MatchingEngine receives snapshots of donations and requests and returns proposed MatchResult values.
6. API returns records and summary information to frontend.

## Constraints
- Managers do not implement JSON serialization or HTTP routes.
- Frontend must not read `data/runtime/*.json` directly.
- MatchingEngine must not mutate input vectors.
- Proposed matches are not proof of acceptance, handover or delivery.
