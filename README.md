# SteamMontage

Tool to create a graphical grid of your Steam games.

Like this:

![montagepreview](https://user-images.githubusercontent.com/18356494/211707819-df77199c-b302-4629-9b7e-716a249f0f4c.jpg)

or even this:

![montagepreviewhuge](https://user-images.githubusercontent.com/18356494/211707910-5241a3cc-fb84-4bdd-af7e-0ecfb83ba839.jpg)

## Get the app

### Binaries

Pre-built binaries can be found on the [releases](https://github.com/grizzlybearington/steam-montage/releases) page.  
Currently, only Windows and Linux are officially supported.

### I want to build SteamMontage myself

Both the Windows and Linux binaries are built on Linux.

#### Dependencies

* [libcurl](https://curl.se/libcurl/libcurl) (32-bit)
* [libjpeg](https://libjpeg-turbo.org) (32-bit)
* [cJSON](https://github.com/DaveGamble/cJSON)
* [whereami](https://github.com/gpakosz/whereami)

#### Linux

Enter the project's root directory (where the Makefile is) and run `make`. You will need the Linux library files.

#### Windows

Uses a cross-compiler, I use i686-w64-mingw32-gcc. Others may work - make sure you update the Makefile. You will also need the Windows library files for the dependencies. See the documentation for each library for more information. Use `make OS=win` to build for Windows.

## Running

You must have an internet connection for SteamMontage to work.

### Options

Options can be set either via command-line arguments or in a **config.txt** file.  
If an option is set as both a command-line argument and in the config file, the command-line argument will be used.  
If an option is set multiple times in the config file, the first one will be used.

At the minimum, SteamMontage will need to be provided with an **API key** and **SteamID**.

**API Key**: A key, dispensed by Steam, used to communicate with their API. More information can be found [here](https://steamcommunity.com/dev/apikey).  
**Steam ID**: Steam IDs come in many forms, SteamMontage is currently only compatible with _SteamID 64_, which is a 17-digit numeric ID. Several tools exist to help you find yours, like [this](https://www.steamidfinder.com/).

#### Command-line Arguments

Command line arguments can be set when calling the program:

```shell
./steammontage -w 20 --steam-id 12345678901234567
```

Command line arguments are detailed below:

| Option      | Description                           |
|-------------|---------------------------------------|
|--api-key    | Sets the API key.                     |
|-h, --help   | Prints usage and exits.               |
|--steam-id   | Sets the Steam ID.                    |
|-v, --version| Displays app version and exits.       |
|-w, --width  | Sets width of montage. Defaults to 10.|

#### config.txt options

config.txt exists as a more convenient alternative to command line arguments. SteamMontage will look for config.txt in the same folder as itself. A normal config file must look something like this:

```properties
#This is a comment!
api-key=ABCDEFGHIJKLMNOPQRSTUVWXYZ123456
steam-id=12345678901234567
width=50
```

config.txt options are detailed below:

| Option        | Description                           |
|---------------|---------------------------------------|
| api-key       | Sets the API key.                     |
| steam-id      | Sets the Steam ID.                    |
| width         | Sets width of montage. Defaults to 10.|

That's it! SteamMontage will take care of the rest!  
SteamMontage will output _montage.jpg_ wherever it is located.

Note: **Any file named _montage.jpg_ (including previous montages) in the SteamMontage folder will be overridden.**

## Things to look out for

1. SteamMontage builds your image **in memory**. Those of us with large libraries will cause SteamMontage to have quite a large memory footprint during its lifetime. The resulting image can also be very large. SteamMontage has been tested with a library as big as 4500 games.

2. JPEG supports a maximum image size of 65,535 pixels in either direction. This is programatically lowered to 65500 to prevent overflows. As each tile is 460x215, this imposes a width limit of 142 and a height limit of <_insert formula here_>. SteamMontage does **not** enforce this during input, and an excessive size will eventually error out.

3. A staggering majority of headers are 460x215. However, some are a different size. Currently, SteamMontage detects these and skips them to prevent issues (and an unsightly montage).

## Future features

* Sorting (by name, playtime, etc.)
* Micro montages
* Game exclusion list
* Game inclusion list (maybe)
* (Your suggestion here..?)

## License

GNU General Public License v3.0 or later.  
Information can be found in [LICENSE](https://github.com/grizzlybearington/steam-montage/blob/main/LICENSE).

SteamMontage is not in any way, shape or form affiliated with Valve Corporation or the Steam platform.
