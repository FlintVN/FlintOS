# Common Installer Demo

Run this bootstrap verification fully automatically. Do not stop after a failed check; continue with fallbacks and produce one final report.

1. Confirm the project root with `pwd` and `git rev-parse --show-toplevel` when available.
2. List installed project skills by scanning directories that contain `SKILL.md`, not with a plain glob that only matches files:
   - Bash/Python fallback:
     ```sh
     python - <<'PY'
     from pathlib import Path
     root = Path('.claude/skills')
     skills = sorted(p.parent.name for p in root.glob('*/SKILL.md')) if root.exists() else []
     print('\n'.join(skills) if skills else 'NO_SKILLS_FOUND')
     PY
     ```
3. List project commands from `.claude/commands/*.md`.
4. Read `.mcp.json` using UTF-8 with BOM support and summarize configured servers:
   ```sh
   PYTHONIOENCODING=utf-8 python - <<'PY'
   import json
   from pathlib import Path
   p = Path('.mcp.json')
   if not p.exists():
       print('NO_MCP_JSON')
   else:
       data = json.loads(p.read_text(encoding='utf-8-sig'))
       servers = data.get('mcpServers', {})
       for name, cfg in servers.items():
           print(f"{name}: {cfg.get('command')} {' '.join(cfg.get('args', []))}")
   PY
   ```
5. Check `uvx` availability:
   - If `uvx` exists, run `uvx --version`, `uvx code-review-graph status`, and if missing/stale explain/run `uvx code-review-graph build` when safe.
   - If `uvx` is missing, try these non-destructive discovery fallbacks before reporting blocked:
     - `python -m uv --version`
     - `python -m pip show uv`
     - check common Windows script paths such as `%APPDATA%\Python\Python312\Scripts\uvx.exe` when accessible.
   - Do not claim MCP is working when `uvx` is unavailable.
6. Run project recommendation again with the common installer when `agent-common-sync.ps1` is reachable from `D:/Workspace/Project/Agents-common-installer/agent-common-sync.ps1`:
   ```powershell
   powershell.exe -NoProfile -ExecutionPolicy Bypass -File "D:/Workspace/Project/Agents-common-installer/agent-common-sync.ps1" -ProjectPath . -RecommendSkills
   ```
7. Final report must include:
   - project root
   - installed skill count and key skills
   - command files
   - MCP servers and whether each can run now
   - recommended skills
   - exact blockers and exact next command to fix them

Keep the report short, but complete. Include exact commands used.
