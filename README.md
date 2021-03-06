Tree structure:

- ```cmake/``` - CMake scripts and source code generators.
- ```hypersomnia/``` - all files needed by the Hypersomnia executable to run properly.
- ```src/``` - complete source code of Hypersomnia, along with 3rd party libraries.
  - ```src/3rdparty``` - 3rd party libraries, upon which the rest of ```src/``` depends.
  - ```src/augs/``` - abstraction of i/o; template code, utility functions, window management.
  - ```src/game/``` - Hypersomnia-specific code that implements the actual game mechanics.
  - ```src/application/``` - functionality specific to Hypersomnia, but not involving the game world itself. Examples: content generation, http server code, the main menu.
- ```todo/``` - a personal to-do list of the founder. At the moment, not meant to be understood by the public.

# Hypersomnia
Community-centered shooter/MMORPG released as free software.

from: http://hypersomnia.pl/summary/

Hypersomnia is an upcoming futuristic universe with elements of fast-paced shooter, stealth and role-playing game.
Set in a hypothetical afterlife reality, it shall provide joy through altruistic behaviours and fulfillment of elaborate social roles,
including, but not limited to, sowing panic as a traitor to benevolent ones.



Decide upon your allegiance to one of the three warring factions whose apple of discord is a disparity between prevailing notions of moral excellence.
**Metropolis. Atlantis. Resistance.**

Watch gameplays on YouTube:

[![IMAGE ALT TEXT](http://img.youtube.com/vi/f0cHnds9UuU/0.jpg)](http://www.youtube.com/watch?v=f0cHnds9UuU "Video Title")
[![IMAGE ALT TEXT](http://img.youtube.com/vi/XsSKj6hJH0w/0.jpg)](http://www.youtube.com/watch?v=XsSKj6hJH0w "Video Title")

![enter image description here][1]
![enter image description here][8]
![enter image description here][3]
![enter image description here][4]

  [1]: http://hypersomnia.pl/pics/summary.png
  [8]: https://gifyu.com/images/16.main_menu_reup.png
  [3]: http://gifyu.com/images/23.light.png
  [4]: http://gifyu.com/images/30.smoke.png

# How to build
To build Hypersomnia, you will need **CMake 3.8** or newer.
You might also need **Python** installed on your system due to requirements of some CMake scripts.
Optionally, you can also install **7-Zip** so that the **Release** configuration can automatically create a compressed archive with the executable and game resources. 

Go to the directory where you wish to have your Hypersomnia project downloaded,
open git bash and paste:

```
git clone https://github.com/TeamHypersomnia/Hypersomnia.git --recursive
```

The repository will start downloading. Once complete, create a ```build/``` folder next to ```CMakeLists.txt``` file. 
Then, use your favorite shell to go into the newly created ```build/``` folder and run:

```
cmake ..
```

If you are on Windows, resultant ```.sln``` and ```.vcxproj``` files should appear in the ```build/``` directory.
Open ```Hypersomnia.sln``` file, select **Release** configuration and hit **F7** to build the game.

As it currently stands, the game is known to build successfully only with Visual Studio 2017 **Preview** under Windows systems. Compilers from Visual Studio 2015 do not support some of the C++ language features.

# Contributing

We fondly welcome every pull request, should it even be a typo fix or a missing const guarantee.

If you however plan to add a completely new feature, please notice us in advance, because the project is continuously in a very, very active development and may undergo a revolution when it is the least expected.
I also recommend that you be familiar with component-based entity architecture beforehand.

You will be added to our TeamHypersomnia organization once we accept at least one of your pull requests.

If you have questions or you fail to build Hypersomnia, ask via mail: patryk.czachurski@gmail.com

Or if you just can't wait to utter some brilliant suggestions regarding the game, please do so, too!

# Launching

You might want to properly configure some variables before launching the game.
- Clone the ```config.lua``` and name it ```config.local.lua``` so that it stays unversioned and unique to your filesystem, if for example you want to preserve your original window resolution and coordinates across further commits.
- The game will try to read ```config.local.lua``` and if there is no such file, it shall try loading ```config.lua```.
