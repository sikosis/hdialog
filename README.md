<p align="center">
  <img src="assets/hdialog.png" width="160" alt="hdialog icon">
</p>

# hdialog

`hdialog` is a Haiku-native command-line utility for presenting custom dialogs
from shell scripts and administration tools. It is inspired by
[swiftDialog](https://github.com/swiftDialog/swiftDialog), but uses the Haiku
API and follows Haiku interface conventions rather than reproducing a macOS
interface.

The current milestone supports a title and message, configurable primary and
secondary buttons, image icons, text and password input, checkboxes, selection
menus, progress, an optional automatic timeout, window dimensions, and useful
process exit codes for scripts.

## Building on Haiku

The native application icon is generated in HVIF format during the build.
Install the conversion tool once if it is not already available:

```sh
pkgman install hvif_tools
```

```sh
make
```

The resulting `build/hdialog` contains its application signature, version, and
HVIF icon resources for Tracker and Deskbar. The artwork source is
`assets/hdialog.svg`; `assets/hdialog.png` is the README rendering.

## Usage

```sh
build/hdialog --title "Maintenance" \
    --message "Updates are ready to install." \
    --button1text "Install" \
    --button2text "Later"
```

The process exits with `0` for the primary button, `2` for the secondary
button or a closed window, and `4` when the timeout expires. Invalid command
line arguments exit with `64`.

Options may be written as `--name value` or `--name=value`. Run
`build/hdialog --help` for the complete list.

An input dialog writes its value to standard output when the primary button is
pressed:

```sh
name=$(build/hdialog --title "Welcome" --textfield "Name:" \
    --button1text "Continue" --button2text "Cancel")
```

Dialogs show the built-in `info` image by default. The scalable built-in images
are `info`, `warning`, `error`, `question`, and `success`:

```sh
build/hdialog --title "Complete" --message "Installation finished." \
    --icon success --iconsize 96
```

Use `--icon /path/to/image` for an image supported by Haiku's Translation Kit,
or `--icon none` for no image. `--iconsize` accepts 32–256 pixels. File images
preserve their aspect ratio. When the path is not a translatable image,
`hdialog` asks `BNodeInfo` for its native Tracker/MIME icon, so applications,
documents, and folders can be used directly. Use `--progress 0` to create a
progress dialog.

Native form controls can be combined in one dialog:

```sh
build/hdialog --title "Connection" \
    --textfield "Password:" --password \
    --checkbox "Remember this setting" --checked \
    --select "Home" --select "Work" --selected "Home"
```

On confirmation, the text value remains the first stdout line. A checkbox adds
`checked=true` or `checked=false`, and a selection menu adds
`selection=Item`. `--select` may be repeated to add menu items.

### Named fields and JSON

Use `--field NAME=LABEL` for repeatable named fields and
`--passwordfield NAME=LABEL` when the value should be hidden. An initial value
can be assigned after its field with `--fieldvalue NAME=VALUE`:

```sh
result=$(build/hdialog --title "Account" \
    --field "username=User name:" \
    --fieldvalue "username=Arthur" \
    --passwordfield "password=Password:" \
    --checkbox "Remember account" \
    --select "Home" --select "Work" \
    --json)
```

Successful JSON output has a stable structure suitable for scripts:

```json
{"fields":{"username":"Arthur","password":"secret"},"checked":false,"selection":"Home"}
```

Without `--json`, named values are written as `name=value`, one per line.
Names must be unique. JSON strings are escaped without requiring an external
JSON library.

## Scripting with `hey`

While a dialog is running, its title, message, progress, and current input can
be addressed through Haiku's standard scripting interface:

```sh
hey hdialog get Title
hey hdialog set Title to "Installing"
hey hdialog set Message to "Downloading packages…"
hey hdialog set Progress to 50
hey hdialog get Input
```

`Title`, `Message`, and `Progress` are readable and writable. `Input` is
read-only. A progress bar must be created at launch with `--progress`; values
set through `hey` must remain between 0 and 100.

## Testing

The command-line parser is independent of the Haiku API, so its tests can run
on Haiku or another system with a C++17 compiler:

```sh
make test
```

## Direction

Planned milestones include JSON input, file pickers, validation rules, and
richer scripted automation.

## Licence

This project is licensed under the MIT License. See `LICENSE`.
