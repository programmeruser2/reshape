# reshape
A conversion tool for developers. 
## Why?
There are many conversion sites that you can use on the web. However, the majority of these aren't open source, and finding them can take some time. By sending your information to them, you may be exposing sensitive data to whoever's runinng the external server.

With `reshape`, conversion is done 100% on your local computer, unless some component explicitly requires network access. Additional components can be loaded with shared libraries, which means you can extend the application using any language that you can compile to a shared library.
## Why AGPLv3?
If a network service is using this, the the users should know what the service is doing with this program so that they know about what the service is actually doing as there is a risk that this could be used to create more converter websites that do malicious actions with user-inputted data.

## Building
Run `make` to build the binaries into `bin/`, including the modules. You will have to edit `/etc/reshape.conf` (or whatever configuration file you use) to point at the shared libraries for the modules.
