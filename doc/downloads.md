FeenoX is distributed under the terms of the [GNU General Public License version 3](https://www.gnu.org/licenses/gpl-3.0.en.html) or (at your option) any later version.

|                                        |                                                       |
|----------------------------------------|-------------------------------------------------------|
|  Debian packages                       | <https://packages.debian.org/unstable/science/feenox> |
|  GNU/Linux binaries                    | <https://www.seamplex.com/feenox/dist/linux>          |
|  Source tarballs                       | <https://www.seamplex.com/feenox/dist/src>            |
|  Github repository                     | <https://github.com/seamplex/feenox/>                 |


 * FeenoX is [cloud-first](https://seamplex.com/feenox/doc/sds.html#cloud-first). It was designed to run on servers.
 * Be aware that FeenoX **does not have a GUI**. Read the [documentation](https://seamplex.com/feenox/doc/), especially the [description](https://www.seamplex.com/feenox/doc/feenox-desc.html) and the [FAQs](https://seamplex.com/feenox/doc/FAQ.html). Ask for help on the [GitHub discussions page](https://github.com/seamplex/feenox/discussions) if you do now understand what this bullet means.
 * Debian/Ubuntu packages are unofficial, i.e. they are not available in `apt` repositories. They contain dynamically-linked binaries and their dependencies are hard-coded for each Debian/Ubuntu release. Make sure you get the right `.deb` for your release (i.e. `bookworm`/`bullseye` for Debian, `kinetic`/`focal` for Ubuntu).
 * Generic GNU/Linux binaries are provided as statically-linked executables for convenience. They do not support MUMPS nor MPI and have only basic optimization flags. Please compile from source for high-end applications. See [detailed compilation instructions](https://seamplex.com/feenox/doc/compilation.html).
 * Try to avoid Windows as much as you can. The binaries are provided as transitional packages for people that for some reason still use such an outdated, anachronous, awful and invasive operating system. They are compiled with [Cygwin](http://cygwin.com/) and have no support whatsoever. Really, really, **get rid of Windows ASAP**.
 
   ```include
   windows.md
   ```
