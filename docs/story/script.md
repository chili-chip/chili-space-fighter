# Script

Every line the game shows. The build reads this file, so editing it and building changes the game
(see [Changing the dialogue](index.md#changing-the-dialogue)). The format is in
[the story overview](index.md#script-format): a `[NAME FRAME]` line, then what the character says
as normal text. The game wraps the text and shows it three lines at a time, ending a page at the end
of a sentence (or after a comma when a sentence is too long for one page). `[STATIC]` has no face.
Lines in `( )` are directions.

Names in boxes: `NOVA`, `AUX-7` (becomes `KIT` in chapter 5), `ROOK`, `BRASK`, `LUMO`, `JUNO`,
`ZIX`, `OSSA`, `DOC`, `WISP`.

## Intro

Hook: `intro`. Shown after PLAY, before wave 1. It opens like the game always did, as the show.

```text
(title card: ULTRA HYPER SPACE / SUPER FIGHTERS / TURBO. EPISODE 1)

[BRASK 1]
Citizens of Halo, good evening. I'm afraid I have to interrupt your programme with some very bad news.

[BRASK 3]
The galaxy is in danger! The Swarm has been sighted at the edge of our Lanes, and the evil aliens are invading as I speak!

[BRASK 4]
But you don't need to be afraid. You never do. Our brave TURBO pilots are already in the air, and they will keep you safe, like they always have.

(cut to cockpit)

[ROOK 1]
And that's our cue. Smile for the camera, rookie. There are ten million screens watching your first flight tonight.

[NOVA 4]
I know! I've watched this show every week since I was six years old. I can't believe I'm actually in it.

[AUX-7 0]
Good morning, Nova. I am AUX-7, your ship computer. Weapons are warm, the hull is polished, and your heart rate is a little high.

[NOVA 3]
That's just excitement. Mostly.

[ROOK 3]
Stay close to my wing and do what I do. Let's give the people at home a good episode.
```

## Chapter 1: The Show

Waves 1 to 10. Halo Station. Boss: the Herald.

### Chapter card

Hook: `chapter_card`, wave 1. In chapter 1 the intro takes the place of `chapter_start`.

```text
CHAPTER 1
THE SHOW
HALO STATION
```

### Radio

Hook: `radio`, wave 5. The Static breaks in for the first time.

```text
[STATIC]
...zzt... is this thing... zzt... Halo, can you hear me?

[LUMO 2]
There we go! Good evening, Halo. This is Lumo, and you're listening to the Static, the only station that isn't trying to scare you.

[LUMO 1]
Quick question while I've got you. Has anyone out there ever actually met one of these evil aliens? We've been looking for ten years and we haven't found a single one.

[ROOK 3]
That's pirate radio, rookie, not the Swarm. They break in every few weeks to stir up trouble. Tune it out and keep your eyes on the targets.
```

### Boss warning

Hook: `boss_warning`, wave 10.

```text
[BRASK 3]
There it is, citizens! The alien mothership itself! TURBO squadron, you are cleared to destroy it. Make it look good!
```

### After the boss

Hook: `after_boss`, wave 10.

```text
[NOVA 4]
We got it! Rook, did you see that? We actually took down the mothership!

[AUX-7 2]
Congratulations, Nova. I am scanning the wreck for survivors, as the rules require. One moment, please.

[AUX-7 4]
That is strange. There is no crew on board. There are no aliens at all. What I am finding is cameras, stage lights, and what appears to be a fog machine.

[NOVA 3]
A fog machine? What would aliens want with a fog machine?

[AUX-7 0]
I don't think they are aliens, Nova. Every part has a Concord serial number. According to its own logs, this ship is called the Herald, and it was built on Halo.

[NOVA 3]
But that would mean it's ours. That we were shooting at our own... AUX, that can't be right.

[BRASK 2]
TURBO Five, this is Marshal Brask. Your computer has picked up some confusing data. Delete that scan, please. Right now.

[AUX-7 1]
The Marshal has the authority to order that. But it is your ship, Nova. Shall I delete it?

[NOVA 2]
No. Keep it. If it's nothing, then nobody should mind us having it.

[BRASK 3]
That was a mistake, rookie. I gave you a ship and a place on every screen in Halo, and this is how you thank me? Wardens, bring that pilot in.

[LUMO 4]
Whoa, whoa! Kid, this is Lumo from the Static. I heard all of that, and I like you already. Fly to the Tollgate, the checkpoint at the end of the Lane, as fast as that ship goes. A friend of mine will meet you there!
```

### Game over

Hook: `game_over`, chapter 1.

```text
[ROOK 2]
That was a bad episode, rookie. Shake it off and we'll shoot it again.
```

## Chapter 2: No Papers

Waves 11 to 20. The Tollgate. Boss: the Tollgate. Juno's Sunset flies on Nova's wing all chapter.

### Chapter card

```text
CHAPTER 2
NO PAPERS
THE TOLLGATE
```

Hook: `chapter_start`, before wave 41.

```text
[JUNO 3]
So you're the one Lumo sent me. A Concord pilot, in a Concord ship, still wearing the white uniform. Lumo has a strange sense of humour.

[NOVA 0]
I'm Nova. I know how it looks, but I'm not with them any more. I came to help.

[JUNO 1]
I'm Juno. I was born in the Yards and I've never had papers, so the Tollgate has never once opened for me. I take the long way round, through the rocks.

[JUNO 3]
Tonight I can't. I've got ten families in the ships behind me, and the Wardens already know we're here. The only way out is through the gate, so we have to break it.

[JUNO 1]
And don't stare at my ship. The Sunset was a show fighter too, until its pilot quit. I bought it for scrap. Now stay on my wing.

[NOVA 2]
Then let's go. I'll fly next to you, and we'll get that gate open together.
```

### Radio

Hook: `radio`, wave 15.

```text
[STATIC]
...zzt...

[LUMO 0]
It's that time of the evening, friends. Every night I read the names of people the Concord sent to work in the Yards, so their families know they're alive. Tonight: Tali. Oren. Little Bex, who is seven and likes to draw.

[LUMO 2]
If you know them, they're thinking of you. And to the convoy at the Tollgate: we're with you. Stay on the signal.
```

### Boss warning

```text
[STATIC]
Papers, please. Papers, please. Ships without papers will be turned back. Papers, please.

[JUNO 3]
That's the gate's own guard robot waking up. It's in our way, Nova. Take it down and the convoy can go through.
```

### After the boss

```text
[JUNO 4]
It's down! The gate is actually down! Convoy, this is Juno. Go, go, go, and don't stop until you see stars you don't know!

[AUX-7 0]
I count forty-two ships going through. Not one of them has papers. All of them have people in them.

[JUNO 2]
You could have run back there, you know. When the big one showed up, I was sure you would. Everyone in white runs.

[NOVA 2]
My whole life they told me the people in those ships were the danger. I just watched a kid wave at me through a window.

[JUNO 4]
Huh. Maybe you're all right after all, white shirt. Come on. If you want to see the factory where those drones are built, I'll show you the Yards.
```

### Game over

```text
[JUNO 3]
Get up, Nova! They're still right behind me and I can't hold them off on my own!
```

## Chapter 3: The Yards

Waves 21 to 30. The Rust Ring. Boss: the Cradle.

### Chapter card

```text
CHAPTER 3
THE YARDS
THE RUST RING
```

Hook: `chapter_start`, before wave 41.

```text
[JUNO 1]
Welcome to the Yards. Every Warden you've ever shot down was built right here, by people who weren't given a choice about it.

[JUNO 3]
I've got a friend on the inside who can help us. Best engineer I know, so be nice.

[ZIX 1]
You are the TURBO pilot. We saw your scan on the Static, all of us, crowded round one small radio. I am Zix.

[NOVA 3]
You're... sorry, it's just that you look exactly like the... like the ones on the...

[ZIX 4]
Like the Swarm on your show. Yes. They drew their monster from my people. I have had five years of that look on your face, from everyone.

[NOVA 3]
I'm sorry. That wasn't fair to you. That was the show talking, and I don't want it talking for me any more.

[ZIX 2]
Good. Then we begin again. Hello, Nova. Now, would you like to know where to shoot the very large ship I was forced to build?
```

### Radio

Hook: `radio`, wave 25.

```text
[ZIX 1]
Nova, I built these drones myself. Their left engines are weak, so they turn badly to the left. I reported it three times and nobody listened. You are welcome.
```

### Boss warning

```text
[ZIX 3]
That is the Cradle. It carries the new Wardens out of the Yards and drops them into the Lanes like eggs. Break it, and the Yards stop.
```

### After the boss

```text
(the overseer's pod ejects and flies off)

[ZIX 2]
Listen to that. The factory has stopped. There will be no more Wardens out of this Yard, not today and not tomorrow.

[ZIX 4]
It is so quiet that I can hear my own shell creak. In ten years of work here, I had never heard it once.

[JUNO 4]
The overseer ran for it, too. Didn't even look back. I think the Yards belong to the people in them now.

[ZIX 1]
I kept one thing from the test bay. A fighter, the green one. They called it Viper. I intend to fly it, although I should warn you I will fly it badly.

[OSSA 1]
Pilots, this is Ossa, leader of the Static. Our base is the Hollow, an old mine hidden in the Drift. You have a home there, if you want one. Come in.
```

### Game over

```text
[ZIX 0]
Do not worry. I have rebuilt worse than this. Hold still and try again.
```

## Chapter 4: Old Wings

Waves 31 to 40. The Drift. Boss: the Paragon, flown by Rook.

### Chapter card

```text
CHAPTER 4
OLD WINGS
THE DRIFT
```

Hook: `chapter_start`, before wave 31.

```text
[OSSA 0]
So. You are the rookie who would not delete the scan. I expected someone taller.

[OSSA 4]
I flew against the Concord before you were born, child. We lost. I have had a long time to think about why.

[OSSA 3]
We do not need heroes here. Heroes burn bright for one battle and then they are gone. We need people who stay. Can you stay?

[NOVA 2]
I don't have anywhere else to be. And I don't think I'd leave if I did.

[DOC 1]
That'll do for an answer. I'm Doc. I patch up whoever comes through that door, so do me a kindness and wear your helmet out there.
```

### Radio

Hook: `radio`, wave 35.

```text
[ROOK 3]
I know you can hear me, Nova. I trained you. I know how you fly, and I know where you'll turn before you do.

[ROOK 1]
Come back with me. Tell them the computer made a mistake. It's not too late to be the hero you wanted to be.

[NOVA 2]
I am being a hero, Rook. That's exactly why I can't come back.
```

### Boss warning

```text
[ROOK 3]
This is the Paragon. The best ship the Concord ever built, for the best pilot it ever had. Last chance, rookie. Stand down.
```

### After the boss

```text
(Rook's pod ejects. Doc's skiff picks it up.)

[DOC 2]
Easy now. You've got a cracked visor and some badly bruised pride, but you'll live. Stop trying to sit up.

[ROOK 4]
Just lock me up and get it over with. I know how this works. I've done it to enough people.

[OSSA 1]
There are no locks here, pilot. That is the door, and it opens from both sides. You may stay or you may go.

[ROOK 2]
I believed them. Every single episode. I thought I was keeping people safe. If the Swarm isn't real, then what have I been doing all these years?

[NOVA 0]
The same as me. I believed it too, right up until a fog machine fell out of a mothership.

[OSSA 3]
So did many of us, once. That is why the doors stay open, Rook. Rest. Decide tomorrow.

[OSSA 3]
And somebody find me a ship. That purple one. It is the ugliest thing I have ever seen, and I love it.

(Ossa takes the Nebula fighter)
```

### Game over

```text
[ROOK 3]
You're still not as good as me, rookie. Come home and I'll teach you the rest.
```

## Chapter 5: The Signal

Waves 41 to 50. Glimmer. Boss: the Prism.

### Chapter card

```text
CHAPTER 5
THE SIGNAL
GLIMMER
```

Hook: `chapter_start`, before wave 41.

```text
[LUMO 3]
Here's my problem, friends. I've been sending that scan to Halo for days, and not one screen has shown it. Something on Glimmer is eating my signal.

[WISP 3]
(the light says) It is the Prism. The Concord cut it out of our moon and made it into a jammer. It screams on every channel, so no other signal can get through.

[AUX-7 0]
I am translating as well as I can. The Glim speak in light, and some of it has no words. Wisp says the moon is in pain.

[WISP 1]
(the light says) Break the Prism, please. Then Lumo's signal will reach Halo, and our moon will be able to sing again.
```

### Radio

Hook: `radio`, wave 45. The Prism sends an override to every ship computer.

```text
[STATIC]
Concord override, priority one. All ship units: lock the controls and return your pilot to Halo. This is not a request.

[AUX-7 4]
...

[NOVA 3]
AUX-7? The stick just went heavy. Are you still with me? Say something.

[AUX-7 0]
I am here. I heard the order, and I understood it perfectly. I have decided that I am not going to follow it.

[AUX-7 1]
Also, while we are on the subject of choosing things. Would you mind calling me Kit? AUX-7 is what they stamped on the box. I picked Kit myself.

[NOVA 4]
Kit. I like it. It's nice to meet you properly, Kit.
```

From here on, every `[AUX-7 n]` box is `[KIT n]`.

### Boss warning

```text
[KIT 3]
That is the Prism, the jammer. Break it, Nova, and Lumo's signal reaches every screen on Halo.
```

### After the boss

```text
[WISP 1]
(the light says) The Prism is broken. Listen. That sound is our moon singing again, and the signal is free.

(the screen fills with soft light for a moment)

[LUMO 4]
We're on! We're on every screen! Halo, can you hear me? It's Lumo, I'm real, and I've got something to show you!

[LUMO 2]
This is a scan from a TURBO pilot's own ship. It's the alien mothership from last week's episode. Have a good look at what's inside.

[KIT 0]
Sending the Herald scan to ten million screens. It is a very clear picture of the fog machine, if I may say so.

[BRASK 3]
Lies! This is Swarm noise, citizens, nothing more! Stay inside your homes and wait for the regular programme to return!

[KIT 2]
I am watching the dock cameras, Marshal. They are not staying inside.
```

### Game over

```text
[LUMO 3]
We've lost the signal... Nova? Nova, if you can hear me, find it again. Please.
```

## Chapter 6: Open Sky

Waves 51 to 60. Halo Station. Boss: the Bulwark, flown by Marshal Brask.

### Chapter card

```text
CHAPTER 6
OPEN SKY
HALO STATION
```

Hook: `chapter_start`, before wave 51.

```text
[BRASK 3]
Citizens, this is an emergency order. As of this moment every gate in Halcyon is closed. This is for your own safety.

[JUNO 3]
He's locking all seven Lanes at once. Nobody goes anywhere, papers or no papers. He'd rather cage the whole system than lose it.

[OSSA 2]
Then we open them. Static, this is Ossa. Every wing, every runner, every ship that flies. We go to Halo!

[ROOK 1]
This is Rook. The Crimson is fuelled and I've had a night to think. If you'll have me, make some room on that wing.

[NOVA 4]
There's always room. Welcome back, wing lead.

[ROOK 4]
No. Just wing, this time. You lead, Nova. I'll follow you in.
```

### Radio

Hook: `radio`, wave 55.

```text
[LUMO 4]
You should see this, Nova! Halo is out in the streets, thousands of them. They're opening the dock gates with their bare hands!

[DOC 1]
And I've just had word from the Rust Ring. The Yards are walking out. All of them, together, singing.
```

### Boss warning

```text
[BRASK 3]
You think those people want freedom, rookie? I've known them for forty years. They want to feel safe, and they want someone to blame. I gave them both!

[NOVA 2]
You gave them aliens to be afraid of. Now they've met Zix and Juno and Wisp, and they aren't afraid any more. It's over, Marshal.
```

### After the boss

Hook: `after_boss` on wave 60 runs this, then `ending`.

```text
(the Bulwark's bridge pod ejects toward Halo)

[KIT 0]
The Bulwark is down. Marshal Brask's pod has landed on Halo, in the main square. He is unhurt.

[LUMO 0]
And the people have him. Not a mob, I'm glad to say. They're setting up a court, a real one, and they want it live on every screen. I think he'll hate that most of all.

[BRASK 2]
You have no idea what you've done. Without me the Lanes will be chaos inside a week.

[OSSA 4]
The Lanes will be full of people, Marshal. It will be loud and it will be difficult. We will manage.
```

## Ending

Hook: `ending`. After wave 60. Then THE END and the credits.

```text
(the seven gate lights go from red to green, one by one)

[KIT 0]
All seven gates are open. I asked each of them what papers they required, and each of them said none.

[JUNO 4]
That's the first time in my life I've flown through a gate instead of around it. It's a lot quicker. I might do it again.

[ZIX 2]
I have news from the Yards. They are building houses with the drone parts. And a concert hall, which nobody needed, so they built it first.

[WISP 1]
(the light says) The moon is singing. Come and hear her, all of you. You are welcome on Glimmer.

[ROOK 2]
I don't really know who I am without the uniform. I've been Rook from the show for so long.

[DOC 1]
Good. That means you get to find out. Take your time. There's no hurry any more.

[LUMO 2]
One last thing before the music. What do we call you lot now? You can't still be Ultra Hyper Space Super Fighters TURBO. The show's over.

[NOVA 4]
No, we're keeping it. All of it. They used that name to sell a lie about monsters.

[NOVA 4]
We're going to use it for the truth: pilots who keep the Lanes open for everyone.

(title card: ULTRA HYPER SPACE / SUPER FIGHTERS / TURBO. EPISODE 2: OPEN SKY)
```

## Free Lanes

Not played in the game yet: story mode ends after the ending, and endless mode has no story. Hook: `free_lanes`. One box at the start of
each boss wave from wave 70 on, in this order, then loop. In story terms: loyalists and runaway
Wardens still roam, and TURBO keeps the Lanes open.

```text
[LUMO 2]
Morning, TURBO! We've got runaway Wardens loose in Lane Three, and a lot of nervous traffic. Are you lot awake?

[JUNO 4]
I've got twenty families going home today, the short way. Would you mind flying ahead of us? Just in case.

[ZIX 1]
Somebody has rebuilt a Cradle from my old plans. They did not even fix the left engine. I am deeply offended. Please destroy it.

[ROOK 1]
That's an old Concord wing coming in. I know some of those pilots. Let me try talking to them before anyone starts shooting.

[OSSA 3]
Remember this, all of you. Open is not a thing you win once. It is a thing you do, every day.

[KIT 0]
I am still here, Nova. I checked, and nobody is ordering me to be. I thought you should know.
```

### Game over in Free Lanes

```text
[KIT 4]
We have lost the hull, Nova, but the Lanes are still open. Shall we go again?
```
