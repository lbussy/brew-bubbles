:::{figure} _static/bb_logo.gif
:align: center
:alt: Brew Bubbles Logo
:scale: 13 %
:::

# Brew Bubbles Documentation

Brew Bubbles is a mature, stable product with no intended roadmap changes. It is maintained for historical reference only. No new releases, upgrades, or individual support are promised. Firmware 2.2.3 is the final supported version.

One of the very first things a homebrewer does in the morning after a brew day is rush to the fermenter and look to see if the airlock is bubbling. Just in case you are not quite at the point you take a day off afterward, or even remember to brew on Saturday, this project is for you.

Brew Bubbles is a combination hardware/software solution for homebrewers. Brew Bubbles intends to monitor the fermentation process by counting and reporting bubbles in an S-type airlock as “Bubbles per Minute” (BPM.) Also, since not everyone has a fancy fermentation chamber and temperature control, Brew Bubbles optionally reports your fermenter’s temperature as well as that of which the room in which the fermenter rests. All of these reports in a compact mobile-responsive web page served by Brew Bubbles.

If you use temperature control already, such as BrewPi Remix or Fermentrack, Brew Bubbles reports that data to display in that solution. If you use a cloud dashboard for your brews, such as Brewer’s Friend, you can display the data there as well.

:::{figure} home.png
:align: center
:alt: Main Window
:scale: 75 %
:::

## Community Channels

Use [GitHub Discussions] for questions and community help. Report reproducible defects through [GitHub Issues]. These channels remain active, but individual support and response times are not promised.

## Table of Contents

```{toctree}
:maxdepth: 2

prerequisites/index
assembly/index
installation/index
setup/index
configuration/index
firmwareupdate/index
wifireset/index
api/index
troubleshooting/index
```

[brewpi remix]: https://www.brewpiremix.com
[fermentrack]: https://www.fermentrack.com
[github discussions]: https://github.com/lbussy/brew-bubbles/discussions
[github issues]: https://github.com/lbussy/brew-bubbles/issues
