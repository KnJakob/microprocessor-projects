#!/usr/bin/env python3
"""
Check hooks in ~/.claude/settings.json vs ./settings.json (in current directory).
If they differ, replace hooks in the global file with those from the repo file,
preserving all other settings.
"""
import json
import os
from pathlib import Path

def load_settings(filepath: Path):
    """Load JSON settings from file, return dict. If file missing or invalid, return empty dict."""
    if not filepath.is_file():
        print(f"Settings file not found: {filepath}")
        return {}
    try:
        with filepath.open("r", encoding="utf-8") as f:
            return json.load(f)
    except json.JSONDecodeError as e:
        print(f"Failed to parse JSON in {filepath}: {e}")
        return {}
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return {}

def cmp_hooks():
    repo_dir = Path(__file__).parent
    repo_settings_path = repo_dir / "settings.json"
    global_settings_path = Path.home() / ".claude" / "settings.json"

    repo_data = load_settings(repo_settings_path)
    global_data = load_settings(global_settings_path)

    repo_hooks = repo_data.get("hooks", {})
    global_hooks = global_data.get("hooks", {})

    # print("=== Hooks comparison ===")
    # print(f"Repo settings ({repo_settings_path}) hooks: {json.dumps(repo_hooks, indent=2)}")
    # print(f"Global settings ({global_settings_path}) hooks: {json.dumps(global_hooks, indent=2)}")

    if repo_hooks == global_hooks:
        print("Hooks are identical – no changes needed.")
        return

    print("Hooks differ – updating global settings with repo hooks...")
    # Update hooks in global data
    if repo_hooks:
        global_data["hooks"] = repo_hooks

    try:
        with global_settings_path.open("w", encoding="utf-8") as f:
            json.dump(global_data, f, indent=2, ensure_ascii=False)
        print(f"Updated {global_settings_path} with repo hooks.")
    except Exception as e:
        print(f"Failed to write global settings: {e}")

if __name__ == "__main__":
    cmp_hooks()
