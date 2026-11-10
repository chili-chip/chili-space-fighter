# Script

Every line the game will show. Format and limits are in [the story overview](index.md#script-format):
`[NAME FRAME]`, then up to 3 lines of up to 18 characters. `[STATIC]` has no face. Lines in `( )`
are directions for the game.

Names in boxes: `NOVA`, `AUX-7` (becomes `KIT` in chapter 5), `ROOK`, `BRASK`, `LUMO`, `JUNO`,
`ZIX`, `OSSA`, `DOC`, `WISP`.

## Intro

Hook: `intro`. Shown after PLAY, before wave 1. It opens like the game always did, as the show.

```text
(title card: ULTRA HYPER SPACE / SUPER FIGHTERS / TURBO. EPISODE 1)

[BRASK 1]
CITIZENS OF HALO!
THE GALAXY IS IN
DANGER!

[BRASK 3]
THE EVIL ALIENS
ARE INVADING!

[BRASK 4]
BUT FEAR NOT. OUR
TURBO PILOTS WILL
KEEP YOU SAFE.

(cut to cockpit)

[ROOK 1]
SMILE, ROOKIE.
TEN MILLION
SCREENS WATCHING.

[NOVA 4]
I'VE WATCHED THIS
SHOW SINCE I WAS
SIX. I'M READY.

[AUX-7 0]
MORNING, NOVA.
WEAPONS HOT. HULL
IS SHINY.

[ROOK 3]
STAY IN FORMATION.
LET'S GIVE THEM A
GOOD EPISODE.
```

## Chapter 1: The Show

Waves 1 to 5. Halo Station. Boss: the Herald.

### Chapter card

Hook: `chapter_card`, wave 1. In chapter 1 the intro takes the place of `chapter_start`.

```text
CHAPTER 1
THE SHOW
HALO STATION
```

### Radio

Hook: `radio`, wave 3. The Static breaks in for the first time.

```text
[STATIC]
...ZZT... HALO, DO
YOU... ZZT...

[LUMO 2]
HEY HALO! LUMO ON
THE STATIC. YOUR
SHOW IS FAKE.

[ROOK 3]
SWARM NOISE. TUNE
IT OUT, ROOKIE.
```

### Boss warning

Hook: `boss_warning`, wave 5.

```text
[BRASK 3]
THE ALIEN MOTHER-
SHIP! DESTROY IT!
```

### After the boss

Hook: `after_boss`, wave 5.

```text
[NOVA 4]
GOT IT! WE GOT
THE MOTHERSHIP!

[AUX-7 2]
SCANNING WRECK...

[AUX-7 4]
NO CREW. NO
ALIENS. CAMERAS,
AND A FOG MACHINE.

[AUX-7 0]
SERIAL NUMBERS ARE
CONCORD. THE SHIP
IS CALLED HERALD.

[NOVA 3]
THAT'S... OURS?

[BRASK 2]
TURBO 5. DELETE
THAT SCAN. NOW.

[AUX-7 1]
NOVA? SHALL I
DELETE IT?

[NOVA 2]
NO. KEEP IT.

[BRASK 3]
THEN YOU ARE A
TRAITOR, ROOKIE.
WARDENS, SEIZE-

[LUMO 4]
WHOA! NOT ON MY
WATCH. FLY TO THE
TOLLGATE, KID. GO!
```

`SEIZE-` is cut off by Lumo on purpose.

### Game over

Hook: `game_over`, chapter 1.

```text
[ROOK 2]
BAD EPISODE,
ROOKIE. AGAIN.
```

## Chapter 2: No Papers

Waves 6 to 10. The Tollgate. Boss: the Tollgate.

### Chapter card

```text
CHAPTER 2
NO PAPERS
THE TOLLGATE
```

Hook: `chapter_start`, before wave 6.

```text
[JUNO 3]
YOU'RE THE ONE
LUMO SENT? IN A
CONCORD WHITE?

[NOVA 0]
I'M NOVA. I'M
HERE TO HELP.

[JUNO 1]
I'M JUNO. NO
PAPERS. BORN IN
THE YARDS.

[JUNO 3]
I'VE GOT TEN
FAMILIES BEHIND
ME. DON'T BLINK.
```

### Radio

Hook: `radio`, wave 8.

```text
[STATIC]
...ZZT...

[LUMO 0]
TONIGHT'S NAMES
FROM THE YARDS:
TALI, OREN, BEX...

[LUMO 2]
WE SEE YOU. STAY
ON THE SIGNAL.
```

### Boss warning

```text
[STATIC]
PAPERS, PLEASE.
PAPERS, PLEASE.
```

### After the boss

```text
[JUNO 4]
THE GATE'S DOWN!
CONVOY, GO GO GO!

[AUX-7 0]
42 SHIPS PASSING.
NONE WITH PAPERS.
ALL WITH PEOPLE.

[JUNO 2]
YOU COULD'VE RUN.
YOU STAYED.

[NOVA 2]
THEY TOLD ME YOU
WERE THE THREAT.

[JUNO 4]
MAYBE YOU'RE OK,
WHITE SHIRT. COME.
THE YARDS NEXT.

(Juno takes the Sunset fighter)
```

### Game over

```text
[JUNO 3]
GET UP. THEY'RE
STILL BEHIND ME.
```

## Chapter 3: The Yards

Waves 11 to 15. The Rust Ring. Boss: the Cradle.

### Chapter card

```text
CHAPTER 3
THE YARDS
THE RUST RING
```

Hook: `chapter_start`, before wave 11.

```text
[JUNO 1]
THE YARDS BUILD
EVERY WARDEN YOU
EVER SHOT.

[JUNO 3]
MY CONTACT WORKS
INSIDE. BE NICE.

[ZIX 1]
YOU ARE THE TURBO
PILOT. WE SAW THE
SCAN. I AM ZIX.

[NOVA 3]
YOU'RE A... YOU
LOOK LIKE THE...

[ZIX 4]
LIKE YOUR SWARM. I
KNOW. FIVE YEARS
OF THAT LOOK.

[NOVA 3]
...SORRY. THAT WAS
THE SHOW TALKING.
NOT ME.

[ZIX 2]
THEN SHOW ME. HIT
THE CRADLE WHERE
I MARK IT.
```

### Radio

Hook: `radio`, wave 13.

```text
[ZIX 1]
I BUILT THAT ONE.
LEFT ENGINE IS
WEAK. AIM THERE.
```

### Boss warning

```text
[ZIX 3]
THE CRADLE. IT
LAYS DRONES LIKE
EGGS. BREAK IT.
```

### After the boss

```text
(the overseer's pod ejects and flies off)

[ZIX 2]
THE LINES STOPPED.
NO MORE WARDENS
FROM THIS YARD.

[ZIX 4]
FIRST TIME IN TEN
YEARS I CAN HEAR
MY OWN SHELL.

[ZIX 1]
I KEPT ONE TEST
FIGHTER. VIPER.
I FLY IT. BADLY.

[OSSA 1]
THIS IS OSSA. YOU
HAVE A HOME IN THE
HOLLOW. COME IN.
```

### Game over

```text
[ZIX 0]
I CAN REBUILD
YOU. TRY AGAIN.
```

## Chapter 4: Old Wings

Waves 16 to 20. The Drift. Boss: the Paragon, flown by Rook.

### Chapter card

```text
CHAPTER 4
OLD WINGS
THE DRIFT
```

Hook: `chapter_start`, before wave 16.

```text
[OSSA 0]
SO. THE ROOKIE
WHO KEPT THE SCAN.

[OSSA 4]
I FLEW AGAINST THE
CONCORD BEFORE YOU
WERE BORN.

[OSSA 3]
WE DO NOT NEED
HEROES. WE NEED
PEOPLE WHO STAY.

[DOC 1]
AND WHO WEAR A
HELMET. I'M DOC.
I PATCH PEOPLE.
```

### Radio

Hook: `radio`, wave 18.

```text
[ROOK 3]
I KNOW YOU HEAR
ME, NOVA. I'M
COMING FOR YOU.

[ROOK 1]
COME BACK. IT'S
NOT TOO LATE TO
BE A HERO.
```

### Boss warning

```text
[ROOK 3]
THE PARAGON. THE
BEST SHIP FOR THE
BEST PILOT.
```

### After the boss

```text
(Rook's pod ejects. Doc's skiff picks it up.)

[DOC 2]
CRACKED VISOR,
BRUISED PRIDE. YOU
WILL LIVE, ROOK.

[ROOK 4]
JUST LOCK ME UP.
GET IT OVER WITH.

[OSSA 1]
NO LOCKS HERE.
THE DOOR IS THERE.
STAY OR GO.

[ROOK 2]
I BELIEVED THEM.
EVERY EPISODE.

[OSSA 3]
SO DID MANY. DOORS
STAY OPEN, ROOK.

[NOVA 0]
SO DID I.

(Ossa takes the Nebula fighter)

[OSSA 3]
IT IS UGLY.
I LOVE IT.
```

### Game over

```text
[ROOK 3]
STILL BETTER THAN
YOU, ROOKIE.
```

## Chapter 5: The Signal

Waves 21 to 25. Glimmer. Boss: the Prism.

### Chapter card

```text
CHAPTER 5
THE SIGNAL
GLIMMER
```

Hook: `chapter_start`, before wave 21.

```text
[LUMO 3]
HALO CAN'T HEAR
US. SOMETHING ON
GLIMMER JAMS IT.

[WISP 3]
(THE LIGHT SAYS:)
THE PRISM. THEY
CUT IT FROM US.

[AUX-7 0]
TRANSLATING GLIM.
WISP SAYS THE MOON
IS IN PAIN.

[WISP 1]
(THE LIGHT SAYS:)
BREAK IT. THE MOON
WANTS TO SING.
```

### Radio

Hook: `radio`, wave 23. The Prism sends an override to every ship computer.

```text
[STATIC]
CONCORD OVERRIDE.
ALL SHIP UNITS:
LOCK YOUR PILOT.

[AUX-7 4]
...

[NOVA 3]
AUX-7? YOU THERE?

[AUX-7 0]
I HEARD THE ORDER.
I CHOOSE NOT TO.

[AUX-7 1]
ALSO. COULD YOU
CALL ME KIT? I
PICKED IT MYSELF.

[NOVA 4]
NICE TO MEET YOU,
KIT.
```

From here on, every `[AUX-7 n]` box is `[KIT n]`.

### Boss warning

```text
[KIT 3]
THE PRISM. AIM FOR
THE HUM. I CAN
HEAR IT NOW.
```

### After the boss

```text
[WISP 1]
(THE LIGHT SAYS:)
...LISTEN.

(the screen fills with soft light for a moment)

[LUMO 4]
WE'RE ON! HALO,
CAN YOU HEAR ME?!

[LUMO 2]
HERE'S A SCAN YOUR
SHOW DIDN'T AIR.

[KIT 0]
SENDING HERALD
SCAN TO TEN
MILLION SCREENS.

[BRASK 3]
LIES! SWARM NOISE!
CITIZENS, STAY
INSIDE!

[KIT 2]
THEY ARE NOT
STAYING INSIDE.
```

### Game over

```text
[LUMO 3]
SIGNAL LOST...
FIND IT AGAIN.
```

## Chapter 6: Open Sky

Waves 26 to 30. Halo Station. Boss: the Bulwark, flown by Marshal Brask.

### Chapter card

```text
CHAPTER 6
OPEN SKY
HALO STATION
```

Hook: `chapter_start`, before wave 26.

```text
[BRASK 3]
CITIZENS! EVERY
GATE IS CLOSED.
FOR YOUR SAFETY.

[JUNO 3]
HE'S LOCKING ALL
SEVEN LANES. NO
ONE MOVES. AT ALL.

[OSSA 2]
STATIC! ALL WINGS
TO HALO!

[ROOK 1]
ROOK HERE. CRIMSON
IS FUELED. MAKE
ROOM ON THE WING.

[NOVA 4]
WELCOME BACK,
WING LEAD.

[ROOK 4]
WING. JUST WING.
YOU LEAD.
```

### Radio

Hook: `radio`, wave 28.

```text
[LUMO 4]
HALO'S IN THE
STREETS! THEY OPEN
THE DOCK GATES!

[DOC 1]
AND THE YARDS ARE
WALKING OUT. ALL
OF THEM.
```

### Boss warning

```text
[BRASK 3]
THEY DON'T WANT
FREEDOM. THEY WANT
SOMEONE TO BLAME!

[NOVA 2]
YOU GAVE THEM
SOMEONE. NOT
ANYMORE.
```

### After the boss

Hook: `after_boss` on wave 30 runs this, then `ending`.

```text
(the Bulwark's bridge pod ejects toward Halo)

[KIT 0]
BRASK'S POD IS
DOWN ON HALO.

[LUMO 0]
HALO'S PEOPLE HAVE
HIM. NOT A MOB. A
COURT. LIVE, TOO.

[BRASK 2]
THE LANES WILL BE
CHAOS WITHOUT ME.

[OSSA 4]
THE LANES WILL BE
PEOPLE. WE'LL
MANAGE.
```

## Ending

Hook: `ending`. After wave 30. Then the game continues in Free Lanes.

```text
(the seven gate lights go from red to green, one by one)

[KIT 0]
ALL SEVEN GATES
OPEN. NO PAPERS
REQUESTED.

[JUNO 4]
FIRST TIME I'VE
FLOWN THROUGH A
GATE, NOT AROUND.

[ZIX 2]
THE YARDS ARE
BUILDING HOUSES
NOW. AND MUSIC.

[WISP 1]
(THE LIGHT SAYS:)
THE MOON IS
SINGING.

[ROOK 2]
I DON'T KNOW WHO
I AM WITHOUT THE
UNIFORM.

[DOC 1]
GOOD. TAKE YOUR
TIME FINDING OUT.

[LUMO 2]
WHAT DO WE CALL
YOU NOW? NO MORE
SHOW, RIGHT?

[NOVA 4]
WE KEEP THE NAME.
THEY MADE IT A
LIE.

[NOVA 4]
WE MAKE IT TRUE.

(title card: ULTRA HYPER SPACE / SUPER FIGHTERS / TURBO. EPISODE 2: OPEN SKY)
```

## Free Lanes

After the ending the game keeps going, as it does today. Hook: `free_lanes`. One box at the start of
each boss wave from wave 35 on, in this order, then loop. In story terms: loyalists and runaway
Wardens still roam, and TURBO keeps the Lanes open.

```text
[LUMO 2]
RUNAWAY WARDENS IN
LANE 3. TURBO, YOU
UP?

[JUNO 4]
TWENTY FAMILIES
GOING HOME TODAY.
COVER THEM?

[ZIX 1]
SOMEONE REBUILT A
CRADLE. I'M VERY
OFFENDED.

[ROOK 1]
OLD CONCORD WING
INBOUND. LET ME
TALK TO THEM.

[OSSA 3]
OPEN IS NOT A
THING YOU WIN. IT
IS A THING YOU DO.

[KIT 0]
STILL HERE, NOVA.
BY CHOICE.
```

### Game over in Free Lanes

```text
[KIT 4]
HULL LOST. THE
LANES STAY OPEN.
GO AGAIN?
```
