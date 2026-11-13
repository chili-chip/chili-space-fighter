# Ships

All ships come from `assets/ships.png`, a 10 by 10 grid of 8 by 8 sprites. Sprite index =
`row * 10 + column`. Indices below match the code, so nothing has to move when the story goes in.

## TURBO fighters (player)

The five fighters were built for the show: fast, loud and very shiny. Each has three frames
(tilt left, centre, tilt right). The player picks one in Options. In the story, Nova takes the
chosen one when leaving Halo, and the others end up with the Static.

| Ship | Frames | Index (centre) | Story |
| --- | --- | --- | --- |
| **Silver** | ![Silver](img/ship-silver.png) | 1 | The rookie's ship, Nova's by default. Plain, reliable, the show's "everyone" fighter. |
| **Sunset** | ![Sunset](img/ship-sunset.png) | 11 | Its pilot quit the show and sold it for scrap. Juno flies it. In chapter 2 it joins Nova's wing when Juno meets Nova (wave 14) and stays to the end of the chapter (it can't be hit and fires when you fire). |
| **Viper** | ![Viper](img/ship-viper.png) | 21 | Taken from a Yards test bay in chapter 3. Zix rebuilds it and flies it badly but happily. |
| **Nebula** | ![Nebula](img/ship-nebula.png) | 31 | Was a show prop, never flown. Ossa takes it in chapter 4: "It is the ugliest thing I have ever seen, and I love it." |
| **Crimson** | ![Crimson](img/ship-crimson.png) | 41 | Rook's ship. In chapter 1 it flies on Nova's wing, the same way the Sunset does in chapter 2. Grounded after chapter 4. Rook flies it again in chapter 6, on the other side. |

## Warden drones (enemies)

Every enemy in the waves is a **Warden**: an unmanned Concord drone. They are built in the Yards by
Outlanders. For the show, the Concord calls them "the Swarm". Pilots use the nicknames below.

| Sprite | Name | Index | Model | What it does in the game | Lore |
| --- | --- | --- | --- | --- | --- |
| ![Drone](img/warden-drone.png) | **Drone** | 4 | W-1 Sentry | No gun, fast, comes in numbers | The cheapest Warden. Was a camera drone before the show. Its heart shape was designed to look "friendly". |
| ![Bug](img/warden-bug.png) | **Bug** | 6 | W-2 Picket | Fires straight down | Lane patrol. Stops ships without papers and calls in the rest. |
| ![Wasp](img/warden-wasp.png) | **Wasp** | 17 | W-3 Hunter | Fires at the player | Built to chase runners. Painted orange for the show "so the kids can see it". |
| ![Crab](img/warden-crab.png) | **Crab** | 39 | W-4 Shell | Fires a 3-way spread every 3 s | Yards guard. Its dome shield is made from Glimmer crystal. |
| ![Tank](img/warden-tank.png) | **Tank** | 47 | W-5 Bastion | Slow, tough, fires 2 aimed shots every 4 s | Gate defender. The most expensive Warden. Zix calls it "a wall that flies". |

Things to keep true when the story goes in:

- Wardens have no crew. The game can show as many explosions as it likes.
- Wardens get faster and tougher every wave. In the story, that is the Concord sending more as the
  Static gets louder.

## Bosses

Each boss is a 16 by 16 sprite. The game shows them in this order, one every tenth wave, and loops after six. In the story
each one is a tool of control, and each fall frees something. Each boss has its own attacks:

| Boss | Attacks, in order |
| --- | --- |
| The Herald | fan, aimed burst |
| The Tollgate | fan, scanner beam, aimed burst |
| The Cradle | egg rain, ring, egg rain, aimed burst |
| The Paragon | aimed burst, beam, fan |
| The Prism | spiral, ring, spiral, beam |
| The Bulwark | twin beam (one from each hull), fan, spiral, egg rain |

A **beam** starts as a thin flickering line from the boss down the screen for 0.75 s. Then it
fires a 5 px beam for about a second. The boss holds still while it fires, so move out of the line.
The Bulwark's two beams leave a gap between them that the ship fits in.

Every boss swings from side to side. Between attacks it may **dash**: it shakes and flashes for
0.3 s, then slides fast to your column. It dashes after about one attack in four, and after about
one in two when enraged (below half health).

| Sprite | Name | Index | Chapter | Pilot | What it controls | What its fall frees |
| --- | --- | --- | --- | --- | --- | --- |
| ![Herald](img/boss-herald.png) | **The Herald** | 64 | 1 | None, remote controlled | The story: a Concord drone carrier dressed as the "Swarm mothership" | The truth: Kit's scan |
| ![Tollgate](img/boss-tollgate.png) | **The Tollgate** | 66 | 2 | None, gate AI | Movement: the gate guardian that checks papers | A convoy of Outlander families |
| ![Cradle](img/boss-cradle.png) | **The Cradle** | 68 | 3 | Yards overseer (ejects) | Work: the carrier that launches Wardens from the Yards | The Yards' workers |
| ![Paragon](img/boss-paragon.png) | **The Paragon** | 84 | 4 | Rook (ejects) | Loyalty: the Concord's best fighter for its best pilot | Rook, in a way |
| ![Prism](img/boss-prism.png) | **The Prism** | 86 | 5 | None, crystal jammer | Speech: blocks the Static's signal to Halo | The signal, and Kit |
| ![Bulwark](img/boss-bulwark.png) | **The Bulwark** | 88 | 6 | Marshal Brask (ejects, arrested) | Everything: the flagship that can lock every Lane | The Lanes |

### The Herald

A big purple wing with orange engines. Before the fight the game calls it the **ALIEN MOTHERSHIP**.
Its real name only shows after Kit's scan. Inside there is no crew and no alien: it is a
remote-controlled Concord drone carrier. Every part has a Concord serial number, its flight recorder
says it was launched from Halo's own dock, and its orders are signed by Marshal Brask's office with
tonight's episode number.

### The Tollgate

Two orange pillars with a scanner eye between them, the gate's own guardian. Its fan of bullets is
the gate "closing". Its line: **"Papers, please."**

### The Cradle

A green arch that carries drones and drops them like eggs. Built by Zix's team, which is why Zix
knows where to hit it.

### The Paragon

A white fighter, the pride of the Concord. Rook flies it. When it enrages (phase 2), that is Rook
taking off the safety limits. Rook ejects when it falls.

### The Prism

A pink crystal with four small crystals around it. Its spiral shots are the jamming signal made
visible. Wisp says the moon is "humming in pain" until it breaks.

### The Bulwark

Brask's flagship: two orange hulls with blue engines, the biggest ship in Halcyon. It can lock all
seven gates at once. Its phase 2 is Brask ordering every lock shut.

## The convoy

The families Juno leads through the Tollgate in chapter 2. After the Tollgate falls, 42 small ships
fly up the screen one by one, behind Nova's ship, with engine flames. There is no convoy art yet, so
the game uses small ships from `assets/ships.png` that are not Wardens, flipped to face up:

| Indices |
| --- |
| 14, 26, 27, 34, 36, 46, 48, 57 |

[Backgrounds](backgrounds.md) still lists a proper convoy ship as art to make.

## Power-ups

Power-ups are supply drops from the Static, fired from runners' ships during the fight. The game
names stay. The story only adds where they come from.

| Power-up | Story |
| --- | --- |
| POWER UP | Weapon parts from Zix |
| RAPID FIRE | A cooler module stolen from a Warden |
| SHIELD | Glimmer crystal plate. Plates stack up to 3 layers; each hit breaks one |
| REPAIR | Doc's patch kit |
| 1UP | A spare fighter from the Hollow |
| BONUS | Concord credits from broken drones, given to the Yards |
