---
title: Download & install FeenoX
lang: en-US
number-sections: true
toc: true
...





> Please note that FeenoX is a **cloud-first** [back end](https://en.wikipedia.org/wiki/Front_and_back_ends) aimed at advanced users.
> It **does not include a graphical interface** and it is not expected to run in Windows.
> See this 5-min explanation about why:
>
> ::::: {.container .text-center .my-5 .ratio .ratio-16x9}
> <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/66WvYTb4pSg?rel=0" allowfullscreen></iframe>
> :::::
>
>
>
> For an easy-to-use web-based front end with FeenoX running in the cloud directly from your browser see either
>  * [CAEplex](https://www.caeplex.com)
>  * [SunCAE](htts://www.seamplex.com/suncae)
>
> ::::: {.container .text-center .my-5 .ratio .ratio-16x9}
> <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/kD3tQdq17ZE?rel=0" allowfullscreen></iframe>
> :::::
>
> Any contribution to make desktop GUIs such as [PrePoMax](https://prepomax.fs.um.si/) or [FreeCAD](https://freecad.org) to work with FeenoX are welcome.


# Debian/Ubuntu install

```
sudo apt install feenox
```

For Debian Trixie backports you will need to add

```
deb http://deb.debian.org/debian trixie-backports main
```

to your APT sources.


| Distribution            | URL                                                      |
|-------------------------|----------------------------------------------------------|
| Debian Trixie backports | <https://packages.debian.org/trixie-backports/feenox>    |
| Debian Forky            | <https://packages.debian.org/forky/feenox>               |
| Debian Sid              | <https://packages.debian.org/sid/feenox>                 |
| Ubuntu                  | <https://launchpad.net/ubuntu/+source/feenox>            |


# Downloads {#sec:downloads}

```{.include}
doc/downloads.md
```

## Statically-linked binaries {#sec:binaries}

```{.include}
doc/binary.md
```

## Compile from source {#sec:source}

```{.include}
doc/source.md
```

## Github repository {#sec:github}

```{.include}
doc/git.md
```

See the [Compilation Guide](doc/compile.md) for details.
Ask in the [GitHub Discussions page](https://github.com/seamplex/feenox/discussions) for help.


# Licensing {#sec:licensing}

FeenoX is distributed under the terms of the [GNU General Public License](http://www.gnu.org/copyleft/gpl.html) version 3 or (at your option) any later version. The following text was borrowed from the [Gmsh documentation](http://gmsh.info/doc/texinfo/gmsh.html#Copying-conditions). Replacing “Gmsh” with “FeenoX” gives:

> ```include
> doc/freesw.md
> ```

```include
doc/licensing.md
```

