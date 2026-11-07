# hdialog

`hdialog` is a Haiku-native command-line utility for presenting custom dialogs
from shell scripts and administration tools. It is inspired by
[swiftDialog](https://github.com/swiftDialog/swiftDialog), but uses the Haiku
API and follows Haiku interface conventions rather than reproducing a macOS
interface.

The first milestone supports a title and message, configurable primary and
secondary buttons, an optional automatic timeout, window dimensions, and
useful process exit codes for scripts.

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

## Testing

The command-line parser is independent of the Haiku API, so its tests can run
on Haiku or another system with a C++17 compiler:

```sh
make test
```

## Direction

Planned milestones include icons via `BNodeInfo`/`BIconUtils`, text fields and
other native controls, progress dialogs updated through `BMessage`, JSON input
and output, and richer scripted automation.

## Licence

This project is licensed under the MIT License. See `LICENSE`.
