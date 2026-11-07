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

```sh
make
```

The resulting executable is `build/hdialog`.

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

Use `--icon /path/to/image` for an image supported by Haiku's Translation Kit,
and `--progress 0` to create a progress dialog.

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

Planned milestones include JSON input and output, multiple named form fields,
file pickers, and richer scripted automation.

## Licence

This project is licensed under the MIT License. See `LICENSE`.
