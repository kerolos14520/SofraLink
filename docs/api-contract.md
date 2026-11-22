# HTTP API Contract — v1.0

Base URL during local development: `http://localhost:8080`. The frontend uses this C++ API only; it must never read `data/runtime/*.json` directly.

| Method | Route | Purpose |
|---|---|---|
| GET | `/api/health` | Health check |
| GET | `/api/donations` | List donations |
| POST | `/api/donations` | Create donation |
| GET | `/api/donations/{id}` | Get one donation |
| PUT | `/api/donations/{id}` | Update donation |
| POST | `/api/donations/{id}/cancel` | Cancel donation |
| GET | `/api/requests` | List requests |
| POST | `/api/requests` | Create request |
| GET | `/api/requests/{id}` | Get one request |
| PUT | `/api/requests/{id}` | Update request |
| POST | `/api/requests/{id}/cancel` | Cancel request |
| POST | `/api/matches/run` | Generate proposed matches |
| GET | `/api/matches` | List saved matches |
| GET | `/api/reports/summary` | Dashboard summary |

## JSON conventions
- Use exact model field names in lowerCamelCase.
- Serialize enum values as UPPER_SNAKE_CASE strings.
- Use `application/json` request/response bodies.
- Match generation returns `Proposed` matches and must not mutate input records or reserve quantities.
- Do not describe a proposed match as a completed handover or delivery.

## Response shape (recommended common envelope)
Success: `{ "success": true, "data": {}, "message": "OK" }`.
Validation failure: `{ "success": false, "errors": ["..." ] }`.
Server failure: `{ "success": false, "message": "Internal server error" }`.

Use appropriate HTTP codes: 200, 201, 400, 404, 422, and 500. Member 1 owns final API implementation and must keep the routes above unchanged unless the team contract is formally updated.
