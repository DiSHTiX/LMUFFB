# Gemini CLI Orchestrator - Architecture & Design

## 1. System Architecture

The system follows a **Controller-Worker** architecture.

```mermaid
graph TD
    User[User Terminal] -->|Starts| Orch[Python Orchestrator]
    Orch -->|Reads| Config[Workflow Config (JSON)]
    Orch -->|Manages| State[State Manager]
    
    Orch -->|Spawns| P0[Phase 0: Researcher]
    Orch -->|Spawns| P1[Phase A.1: Architect]
    Orch -->|Spawns| P2[Phase A.2: Lead Architect]
    Orch -->|Spawns| P3[Phase B: Developer]
    Orch -->|Spawns| P4[Phase C: Auditor]
    
    subgraph "Isolation Boundary"
        P0 -- Writes --> Art0[Research Report]
        P1 -- Reads --> Art0
        P1 -- Writes --> Art1[Plan Artifact]
        P2 -- Reads --> Art1
        P2 -- Verdict --> Orch
        P3 -- Reads --> Art1
        P3 -- Commits --> Git[Git Repository]
        P4 -- Reads --> Art1
        P4 -- Reads --> Git
        P4 -- Verdict --> Orch
    end

    %% Feedback Loops
    Orch -.->|Reject| P1
    Orch -.->|Fail| P3
```

## 2. Component Design

### 2.1 The `AgentWrapper` Class
This class encapsulates the interface with the Gemini CLI.
*   **Responsibilities:**
    *   Constructing the full shell command.
    *   Managing `stdin`/`stdout` piping.
    *   Injecting the "System Prompt" (e.g., "You are a rigid worker. Output JSON only.").
    *   Handling timeouts and process cleanup.

### 2.2 The `PromptBuilder` Module
Responsible for assembling the final prompt string sent to the agent.
*   **Logic:** `Base Prompt` + `Task Context` + `Input Artifacts` + `Output Instructions`.
*   **Example:**
    > "CONTEXT: You are the Developer.
    > INPUT: Read the plan at 'docs/plans/fix_v1.md'.
    > TASK: Implement the code. Run tests.
    > FORMAT: End your response with JSON: { 'status': 'success', ... }"

### 2.3 The `ResponseParser` Module
Responsible for extracting structured data from the unstructured LLM output.
*   **Algorithm:**
    1.  Receive raw stdout string.
    2.  Regex search for the last occurrence of `{...}` structure.
    3.  `json.loads()` the extracted substring.
    4.  Validate against a Pydantic model or schema.

### 2.4 The `WorkflowEngine` Class
The main state machine.
*   **State:**
    *   `current_step`: (enum: RESEARCH, PLAN, PLAN_REVIEW, CODE, CODE_REVIEW)
    *   `workspace_root`: Path to repo.
    *   `artifacts`: Dictionary of paths.
*   **Transitions:**
    *   `RESEARCH` -> `PLAN`
    *   `PLAN` -> `PLAN_REVIEW`
    *   `PLAN_REVIEW` -> `CODE` (if Approved)
    *   `PLAN_REVIEW` -> `PLAN` (if Rejected - Feedback Loop)
    *   `CODE` -> `CODE_REVIEW` (if Git Commit made)
    *   `CODE_REVIEW` -> `MERGE` (if Pass)
    *   `CODE_REVIEW` -> `CODE` (if Fail - Feedback Loop)

## 3. Data Flow

### Phase 0: Research (Optional)
1.  **Orchestrator** checks if `--research` flag is on.
2.  **Orchestrator** spawns **Researcher**.
3.  **Agent** produces `docs/dev_docs/research/report_X.md`.

### Phase A: Planning
1.  **Orchestrator** spawns **Architect**.
    *   *Input:* User Request + Research Report (if any).
2.  **Agent** writes `docs/dev_docs/plans/feature_X.md`.
3.  **Orchestrator** spawns **Lead Architect (Plan Reviewer)**.
    *   *Input:* The Plan File.
4.  **Agent** outputs JSON: `{"verdict": "APPROVE"}` or `{"verdict": "REJECT", "feedback": "..."}`.
    *   *If REJECT:* Loop back to Architect with feedback.

### Phase B: Implementation
1.  **Orchestrator** reads Approved Plan.
2.  **Orchestrator** spawns **Developer**.
3.  **Agent** modifies code, runs tests.
4.  **Agent** commits changes to git.
5.  **Agent** prints JSON: `{"commit_hash": "abc1234", "tests_passed": true}`.

### Phase C: Review
1.  **Orchestrator** spawns **Auditor**.
    *   *Input:* Plan + Commit Hash.
2.  **Agent** writes `docs/dev_docs/reviews/review_X.md`.
3.  **Agent** prints JSON: `{"verdict": "PASS"}` or `{"verdict": "FAIL"}`.
    *   *If FAIL:* Loop back to Developer with Review Report.

### Phase D: Finalization
1.  **Orchestrator** merges branch.
2.  **Orchestrator** moves artifacts to `docs/dev_docs/archived/`.

## 4. Key Decisions & Trade-offs

*   **No "Memory" by default:** We explicitly choose NOT to pass the chat history. If the Developer needs to know *why* the Architect made a decision, they must read the Plan document. This forces better documentation.
*   **Polling vs. Blocking:** Since we wrap the process, we use **Blocking** calls (waiting for the subprocess to finish) rather than Polling. The Agent script itself is responsible for the "Loop" of running tests until they pass.
*   **Error Handling:** If the Agent fails to output JSON, the Orchestrator will assume failure and ask the user for manual intervention or a retry.


