# GWTTT Account Exporter

A read-only Guild Wars 1 plugin for [GWToolbox++](https://www.gwtoolbox.com/) that exports Guild Wars title progress for easy import into [GWTTT](https://github.com/mauirixxx/gwttt).

The exporter reads the currently loaded character's title tracks through GWToolbox/GWCA and includes both **character-specific** and **account-wide** title progress in the export. It does not modify Guild Wars data.

## Requirements

- Guild Wars 1
- GWToolbox++
- `TitlesExport.dll` from this repository's GitHub Actions build

GWToolbox++ is only required for the exporter plugin. It is not required to use GWTTT itself.

## Installation

Download `TitlesExport.dll` from the latest successful GitHub Actions build artifact and place it in your GWToolbox plugins directory. A typical installation is:

`%USERPROFILE%\Documents\GWToolboxpp\<computername>\plugins`

The exact GWToolbox data location can vary, including when Windows/OneDrive redirects the Documents folder. Use the plugins directory belonging to the GWToolbox installation/profile you actually run.

Start Guild Wars and GWToolbox++, then enable the plugin under **GWToolbox → Settings → Plugins**.

## Exporting titles

Load the Guild Wars character whose character-specific titles you want to export, then enter:

`/exporttitles`

The plugin:

- reads the available title tracks for the currently loaded character;
- includes both character-specific and account-wide title progress;
- copies the JSON export to the Windows clipboard; and
- saves the same data as a timestamped file named `GWTTT-Titles-YYYY-MM-DD_HH-MM-SS.json` under the GWToolbox `Exports` folder in Documents.

The timestamp prevents later exports from overwriting earlier ones, so the files can also serve as a simple progression history.

GWToolbox chat reports the path of the saved file after a successful export.

## Importing into GWTTT

Sign in to GWTTT, open **Preferences**, and use **Import Guild Wars titles** to select the exported JSON file. GWTTT previews the changes before applying them and routes account-wide and character-specific title progress to the appropriate records.

The importer uses stable GWCA title IDs rather than depending on GWTTT's display names, so GWTTT can retain customized title names without breaking imports.

## Building

GitHub Actions builds the plugin automatically for changes under `plugin/` and can also be run manually from the repository's **Actions** tab.

The workflow publishes an artifact named `TitlesExport` containing `TitlesExport.dll` (and a PDB when available).

Internally, the source/build target retains the upstream `AccountExport` name because the plugin was bootstrapped from that implementation; the distributable DLL is renamed to `TitlesExport.dll` by the build workflow.

## Privacy and scope

The exporter is intentionally read-only. It exports game progression data needed for GWTTT title tracking and does not export Guild Wars account e-mail addresses or passwords.

## Attribution

The plugin was bootstrapped from Graphmaxer's MIT-licensed `gw1-mcp` AccountExport plugin and uses GWToolbox++/GWCA interfaces. See `LICENSE` for the repository's license and attribution.
