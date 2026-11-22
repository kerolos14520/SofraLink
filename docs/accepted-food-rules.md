# Accepted Food Rules (Sample Academic Rule Set)

> **Status: DRAFT – to be confirmed with Member 4 (Validation).**
> These are **sample rules for the SofraLink university prototype only**. They are **not** the rules of any real humanitarian organization, are **not** a food-safety certification, and must not be presented as official guidance. All data in the prototype is simulated.

## 1. Scope

SofraLink handles **packaged, sealed, non-perishable** food products and tracks them up to **documented handover** to an authorized organization. A proposed match is not approval, handover, or proof of delivery.

## 2. Sample accepted food categories

| Category (display name) | Examples | Typical unit |
|---|---|---|
| Rice | Packaged white or brown rice | kg |
| Pasta | Dried pasta in sealed packs | kg |
| Dried legumes | Lentils, chickpeas, dried beans | kg |
| Flour | Sealed packaged flour | kg |
| Sugar and salt | Sealed packaged sugar or salt | kg |
| Cooking oil | Factory-sealed bottles | l |
| Canned vegetables | Sealed cans | can |
| Canned legumes | Sealed cans | can |
| Canned fish | Sealed cans | can |
| Tomato paste | Sealed cans or jars | can |
| Tea | Sealed packaged tea | box |
| Powdered milk | Factory-sealed packs | kg |

**Open item for Member 4:** the exact category strings and unit strings used by `Validation` must be agreed so that donations and requests use identical values. The names above are display names and are not yet a contract.

## 3. Sample donation acceptance rules

A donated product is acceptable only if **all** of the following hold:

1. It is in **original, factory-sealed packaging** (`isSealed = true`).
2. It is **non-perishable** (`isNonPerishable = true`).
3. It is **not expired**, and the expiry date is on or after a minimum remaining shelf-life threshold (sample value: **at least 90 days** after the registration date; threshold to be confirmed by Member 4).
4. The package is **undamaged** (no leaks, bulging cans, or broken seals) and the label is legible.
5. The quantity is a **positive integer** paired with a `unit`.
6. A `productBatchNumber` is recorded when one is printed on the package.

## 4. Sample exclusions (not accepted)

- Fresh, refrigerated, frozen, or cooked food
- Homemade or unpackaged food
- Opened, partially used, or repackaged items
- Expired items
- Alcohol
- Items with damaged packaging or unreadable labels

## 5. Sample rules for organization requests (RequestManager)

| Field | Rule |
|---|---|
| `organizationName` | Required, non-empty (simulated name only) |
| `category` | Required, non-empty; should match an accepted category |
| `requestedQuantity` | Positive integer; **unchanged after creation** |
| `remainingQuantity` | Set equal to `requestedQuantity` on creation; reduced **only** by allocation |
| `unit` | Required, non-empty; quantities of different units are never added together |
| `urgencyLevel` | Integer 1 (lowest) to 5 (highest) |
| `location` | Required, non-empty (simulated location only) |
| `neededByDate` | Required, valid date. **Format pending Member 1:** `YYYY-MM-DD` (AGENTS.md) or `DD-MM-YYYY` (task PDF) |
| `status` | Starts as `Pending` |

### Request status behavior

| From | Allowed next status | How |
|---|---|---|
| Pending | Approved, Rejected, Cancelled | `updateRequestStatus` / `cancelRequest` |
| Approved | Cancelled | `cancelRequest` |
| Pending / Approved / PartiallyFulfilled | PartiallyFulfilled (remaining > 0) or Fulfilled (remaining = 0) | `allocateQuantity` only |
| PartiallyFulfilled | Cancelled | `cancelRequest` |
| Fulfilled, Rejected, Cancelled | none (terminal) | – |

"Active" requests returned by `getPendingRequests()` are those in **Pending, Approved, or PartiallyFulfilled**.

## 6. Notes

- Cancelling a request changes its status to `Cancelled`; the record is never deleted.
- These rules are a teaching example. A real deployment would need rules defined with and approved by an authorized humanitarian organization and applicable food-safety authorities.
