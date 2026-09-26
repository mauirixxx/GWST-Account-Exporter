# GWST Account Exporter

Read-only Guild Wars 1 / GWToolbox++ plugin for exporting account and character state for GWST.

## Current command

`/exportaccount`

The initial version exports the currently loaded character, professions, level, map ID, unlocked heroes, account-unlocked skills, and character-learned skills to JSON on the clipboard.

## Build

GitHub Actions builds the plugin. Open **Actions → Build GWST Account Exporter** and run the workflow manually, or push changes under `plugin/`.

The artifact is named `GWST-Account-Exporter` and contains `AccountExport.dll`.

Install the DLL in:

`%LOCALAPPDATA%\GWToolboxpp\<computername>\plugins`

Then enable it under GWToolbox **Settings → Plugins**.

## Planned GWST extensions

- opaque Guild Wars account UUID
- title tracks and title points
- mission and bonus completion, including Hard Mode
- vanquished areas
- unlocked maps/outposts
- faction totals
- experience and skill points
- other useful character/account progression

The exporter will deliberately avoid account email and password data.

## Attribution

The bootstrap exporter derives from Graphmaxer's MIT-licensed `gw1-mcp` AccountExport plugin. See LICENSE.
