// license:BSD-3-Clause
// copyright-holders:David Haywood, Palindrome, Roberto Fresca
/****************************************************************************************************************

    Aristocrat MK5 / MKV hardware
    possibly 'Acorn Archimedes on a chip' hardware

    Note: ARM250 mapping is not identical to plain AA

    Current significant issues:
     - Games run twice as fast as they should, sound effects are double speed etc.
       There are threads that say when running in VGA mode an original AA
       will play music etc. at half the expected speed, so it is likely
       that the way the timers work differs in this mode (25Hz instead of 50?)
     - Sounds are being output as bleeps and bloops in the older games and New Zealand/touchscreen games.
     - Games occasionally give a coin diverter fault when inserting coins, mainly with US region games.
     - Early US games will lock up completely if a hand pay is performed, requiring a complete memory reset.
       To avoid this until the issue has been fixed, change the hopper and jackpot limits to a very large number
       such as 999999 so large wins will be paid via the hopper.
       US games where the music/sound works do not have this problem.
     - Venezuelan games give a note acceptor error on boot even if the note acceptor is disabled in the options
     - qnilebr (actually the 0301718V BIOS itself) won't accept coins on boot until the jackpot reset key is toggled (bug or not?)
     - Later games from NSW/ACT and Venezuela lock up (hang) after a while after a certain amount of spins (usually 50 spins)

    US games which can completely freeze after exceeding $1199.99 and forcing a hand pay, requiring the SRAM to be wiped:
     - bumblbugu, bumblbugua, chickna5u, chickna5ua, eforsta5u, eforsta5ua, mgarden, minemineu, minemineua, pengpayu,
       pengpayua, pengpayub, swhr2u, trpdlghtu, wcougaru, wcougarua, wcougarub, wnpost

    Games which do *not* lock up after a certain amount of spins:
     - All games from Holland, New Zealand, USA, and Casino versions.
     - baddog, buttdeli, cashcham, chickna5, dmdfever, dolphntra, dolphntrb, drgneye, dstbloom, dstblooma,
       eforsta5, jumpjoey, kgalaha, kgbirda5, locoloot, locoloota, lonewolf, luckyclo, mammothm, minemine,
       mountmon, mountmona, mystgard, oscara5a, pengpaya, pengpayc, phantpay, przfight, qnileb, qtbird,
       reelpwr, retrsama, retrsamb, rushrst, snowcat, swhr2a, thndh, thndha, thor, topbana,
       trpdlght, wamazona, wcougar, wildbill, wldangel

    BIOS ROMs are actually nowhere to be found on a regular MK5 system, as the BIOS code is at the start of game ROMs in U7 and U11.
    Therefore, no BIOS system is required as it is built into each game, with the exception of casino software (see below).
    On most US machines, set chips are required to change the system configurations on a game by swapping them with the game ROMs in U7/U11.

    Casino versions actually do have a BIOS, otherwise known as a Base System, which is installed at U7/U11 at all times.
    Casino game EPROMs are loaded in U8/U12 and beyond.

    Casino games (except qnilebr), as well as games from Queensland and Victoria, require a comms protocol to be emulated,
    otherwise they will remain in a disabled state and cannot be played.
    Only New Zealand (0700474V) and Brazilian (0301718V) casino BIOSes have been dumped so far.

    The Brazilian casino BIOS does not use comms, therefore qnilebr is playable. By swapping u7/u11 with the other
    casino games (goldpyrb/jungjuic/penpir2), these games also become playable.

    Queensland games and the 0700474V casino BIOS all use QCOM, blackpnt uses VLC (Video Lottery Consultants) comms instead.

    Diamond Touch, Dream Weaver, Magic Touch and the New Zealand games have an autoplay option.
    The New Zealand games have this enabled by default in the options, whereas on the NSW games it is disabled by default.
    The Autoplay button replaces the fourth play line button normally used for 7 or 15 lines (orchidmsnz has 8 lines/Autoplay due to its unique reel layout).

    Some Venezuelan games have a 'Play maximum lines' option, this will turn every play line button into maximum lines.
    For example, the default setup may have 1, 5, 10, 15 and 20 line buttons; this option will turn every play button into 20 lines.

    The gamble (double up) feature can be enabled in the options on non-US machines. It is disabled by default.
    On US machines which don't use set chips, the gamble feature is enabled with dip switch 2-1.
    On US machines which use set chips, the gamble feature can be enabled in the set chip options, but only if the game's region allows it.
    The gamble option can not be enabled at all in the Brazilian casino BIOS, although the code is still in place.

    The standard double up mode on Aristocrat games is the red/black card game, which is in fact the only option on US games.
    Some machines have different gamble features, such as being able to bet on the four card suits for 4:1 odds,
    or to spin a single slot reel which has 2:1, 3:1, 5:1, 10:1 or 100:1 odds of landing on the middle line.
    Other games replace the cards with animations, for example the double up game in Prize Fight bets on which
    boxer will knock out the other, likewise in Sumo Spins one sumo wrestler will ring-out the other.
    In both Prize Fight and Sumo Spins the two opponents are wearing either red or black just like the cards they replaced.
    The gamble feature is not available if a win coincides with a jackpot pay such as a Hyperlink feature.

    Regional button layout differences:
    US games have the payline buttons on the top row, the player selects the number of lines to be played first before choosing the bet multiplier to spin.
    Some US games can have the bet and play line buttons reversed in the set chip options, which creates the same type of button layout as non-US games.
    Non-US games have the bet buttons on the top row, the player selects the bet multiplier first before selecting the amount of lines to play.
    An exception to the non-US layout applies if the game only has one payline, for example wamazona. In this case, the bottom row is used for the bet multipliers.
    Some non-US games default to 1 credit per line when there are zero credits in the machine so that a player does not accidentally bet higher than intended.
    The Chariot Challenge (both sets) also reverts to 1 credit per line after rebooting, regardless of the previous bet.

    Some games can be set up to multiple bet and line configurations. Usually this applies to the US set chip games,
    however some non-US games also have this option, such as baddog, marmagic, trojhors and tritreat.
    Multiple button panels are supported as artwork files and can be toggled in MAME's Video Options menu.

    Some early games such as swhr2a have an option to have either music or coin sounds to be played during a win.
    This option is in the Sound System setup rather than in Machine Options.
    Selecting "Base" plays coin jingles while selecting "MK2.5" plays a small selection of prerecorded music taken from MK2.5 games.

    Later non-US games moved the collect limit from the Machine Options menu into its own menu.

    US Hyperlink (e.g. Cash Express) games will not trigger the jackpot feature if variation 5 is not selected in the set chip options.
    This is not a bug, these games can either be set to normal standalone games or Hyperlink jackpot games.
    This affects dolphntrce, dolphntrcea, dolphntrceb, eforsta5ce, glizrdce, kgalahce, pengpuck, qnilece, qnilecea and qnileceb.

    Non-US Hyperlink games will still trigger the jackpot feature as intended, however the link system is not emulated
    therefore no jackpot credits are paid if the Hyperlink feature is triggered, and the games will need the jackpot key to be toggled to continue play.
    If the link system is offline on a real machine, the game will disable itself after the Hyperlink feature and a hand pay of the applicable jackpot amount
    would be required. Normally if this occurs in the wild the machine is immediately taken out of service until the link system is working again.

    List of Hyperlink systems/themes on MK5 hardware:
    Cash Express - Train theme.
    Penguin Pucks - Arctic/Antarctic theme, based on Cash Express.
    Maximillions - Game show style theme, stylized as Maximillion$. A similar game for the US market is Millioniser, stylized as Millioni$er, however this may only be on later hardware.
    Scorchin' Fortune - Sports car/racing theme, based on Cash Express.
    Fast Lane - Same theme as Scorchin' Fortune but the jackpot feature is slightly different.
    Born To Be Wild - Motorcycle theme, based on Cash Express, later recycled on MK6 hardware as Thunderheart (not to be confused with the game Thunder Heart).

    Note: The Hyperlink jackpot feature trigger is won at random and is predetermined the instant the player has started a game.
    Pressing the buttons to stop the Hyperlink reels is only a visual effect with no skill involved, likewise touching the icons to reveal the characters in Maximillions.
    The jackpot level won (e.g. Grand, Major, Minor or Mini) is also predetermined at the start of the game.

    Most Hyperlink games have a set of four eight-digit, seven-segment LEDs installed in the topbox for displaying the progressive jackpots.
    The Grand and Major jackpot displays are larger than the Minor and Mini jackpot displays.

    There is a discrepancy with some game names between the ROMs and the artwork or even official documents:
    For example, swhr2 is called Sweethearts II inside the ROM, however on the artwork it is called Sweet Hearts II.
    Mountain Money displays "MOONSHINE MONEY" when a win with the wild Moonshine occurs. The game itself is not called Moonshine Money.
    Chicken displays "Chicken Run Feature Completed" at the end of the feature. The game itself is not called Chicken Run.
    Thunder Heart is named Thunderheart on the artwork, as well as on the later Hyperlink jackpot games.
    Golden Pyramids is sometimes called Golden Pyramid (without the trailing 's') in the ROM.

    Some games also have completely different artwork (using the same theme and paytable) but use the ROMs from another game. Examples are:
    Heart Throb = Sweethearts II (Heart Throb confirmed using 0200004V EPROMs)
    Moon Fire = Indian Dreaming (Moon Fire confirmed using 0100845V EPROMs)
    Golden Pyramids = Queen of the Nile (Golden Pyramids confirmed using 0300439V EPROMs; note that some ROMs actually do contain the Golden Pyramids string)

    Note that the artwork for Golden Pyramids (NSW/ACT) has a 1996 copyright, whereas Queen of the Nile has a 1997 copyright.
    Earlier versions of Golden Pyramids (undumped) have prerecorded win music from MK2.5/MK4 games similar to dolphntra, as with other early MK5 games.
    Queen of the Nile does not use this early prerecorded music in any of its variants.

    List of clones (identical games with different names and/or graphics):
    Queen of the Nile = Golden Pyramids = Dolphin Treasure = Beyond the Reef (undumped) = Adonis = Geisha (note that Adonis has a slightly different symbol layout for reel 1, but the symbol frequency is still identical)
    Sweethearts II = Heart Throb = Desert Bloom = Tropical Delight = Coral Riches II
    Phantom Pays = Enchanted Forest = Magic Garden = Lucky Clover = The Gambler = Thunder Heart (9 line) = Mountain Money (9 line)
    Diamond Touch = Magic Mask = Party Gras = Diamond Destiny
    Bumble Bugs = Oscar
    Butterfly Delight = Golden Canaries (undumped)
    Cash Cat = Kooka Bucks
    Cash Crop = Money Mouse
    Unicorn Dreaming = Inca Sun
    Reelin-n-Rockin = Indian Dreaming = Moon Fire
    Reel Power = Wizard Ways
    Wicked Winnings = Way To Go (undumped)
    Snow Cat = Loco Loot = Wild Coyote
    Sweet Liberty Deluxe = Koala Mint
    Cash Chameleon = Canyon Rose = Queens of Cash
    Green Lizard = King Galah
    Penguin Pays = Jumping Beans = Jumpin' Joey = Fortune Fever = Honky Tonk
    Top Banana = Black Panther
    Panther Magic = Black Rhino (undumped) = Wild Africa = K.G. Bird = Wild Cougar = Thor = Fantasy Fortune (undumped)
    Wild Thing = Return of the Samurai
    Toucan Tango = Boot Scootin'
    Gnome Around the World = Penguin Pirate = Penguin Pirate II
    Prize Fight = Super Bucks II
    Silver Wolf = Lone Wolf
    Mystic Garden = Mountain Money (20 line) = Mammoth Money
    Winning Post = The Chariot Challenge
    Rushin' Rooster = Chicken
    Yukon Gold = Margarita Magic
    Peacock Flutter = Treasure Trove = Trojan Horse
    Mine Mine Mine = 3 Bags Full (undumped) = Fortune Hunter (only released on MK4)
    Magic Touch = Dream Weaver
    Keep Your Hat On = Bachelorette Party

*****************************************************************************************************************

    How to set up the games from scratch:

    Standard NSW/ACT games and similar non-US games, including Holland and Venezuela:

    Step 1: Audit key in (F2), press Reserve (A) and the fourth line button (G) together to clear the memory. Some games may require the main door to be open (press M) prior to clearing.
    Note: On 3-payline games, press Reserve (A) and Bet 1 Credit (E) to clear the memory.

    If open, the main door can be closed from this point on (press M again).

    Step 2: Enter Operator Setup -> Machine Options

    Usually, the Machine ID can be set to anything, however some games complain if it set to zero.
    Base Credit Value is the denomination e.g. $0.01 is a one cent machine; $1.00 is a one dollar machine. Some machines may not allow certain values, others may only have one setting.
    Token Value is the coin used. The default coin in Australian machines is $1.00 ($2.00 for New Zealand) but there are a number of options from 1 cent all the way to 100 dollars.
    Percentage Variation is how loose or tight you want the machine to be, the higher the percentage, the better the game is to the players at the cost of the house edge. Usually the default is "Variation 99" or around 87%. Again, some machines may only have the one option.
    CCCE is for communications in a gaming venue, and is not required for emulation.
    Collect limit is the highest amount of credits allowed to be cashed out before a hand pay is required.
    Hopper refill is the amount of coins the machine should receive every time when the hopper is empty (not required for emulation as the hopper has infinite coins!)
    Gamble is the double up feature, and it is disabled by default as seen in the diagram. Change the setting to YES for the ability to bet against your wins; if disabled, the wins are automatically added to the credit meter.

    Step 3: Once everything has been set up, open the Security Cage/Logic Door (L) and press (W) to save (or (R) on 3-payline games; (H) on single-line games), and close the logic door (L again).
    By now it should be safe to key out (F2) and the game should be ready to accept credits, if not, check whether the main door (M) is still open or if it needs another memory reset (if pressing F2 went to the main menu instead of going back to the game).

    All items with ?? must be changed or the game will not allow you to save any settings.

    Later games may have more options, such as the ability to toggle between a printer and/or hopper. Disabling the printer will enable the hopper refill option.
    Some games may enter a graphical menu for the bet/line/denomination setup, usually if they support multiple betting options.

    Note: To disable both the hopper and forced hand pay on cashout, change both the Collect Limit and Hopper Refill to $0.00.
    Disabling this setting will allow hand pays but still allow the player to continue if they inadvertently hit Collect.
    To perform a hand pay or jackpot reset (e.g. after a Hyperlink feature), press (V) to allow the game to enter play mode.
    Note that in some games such as Treasure Trove (trstrove), the collect options are moved to Operator Setup -> Set Collect Limit rather than in Machine Options.


    New Zealand non-casino games, and most NSW/ACT touchscreen games:

    These games have a slightly updated menu system reminiscent of MK6 games, complete with a black background instead of blue.

    Step 1: Audit key in (F2), press Reserve (A) and the first line button (S) together to clear the memory. The main door does not need to be open.

    Step 2: Enter Operator Setup -> Machine Options

    Step 3: Set everything up as above, open the Security Cage/Logic Door (L), and save the machine options (which now has its own spot on the menu instead of a dedicated button).
    Close the Security Cage (L) and turn off the Audit key (F2) and the game should be ready to accept credits.

    New Zealand machines are usually identical to Australian games except that they normally use NZ $2.00 coins in place of AU $1.00 coins,
    and have smaller but more frequent payouts due to gambling laws which limit the maximum possible win per bought game to $500.00. CCCE comms is not used on New Zealand machines.


    Queensland games, older types:
    bumblbugql, chickna5ql

    Audit key in, press Reserve (A) and the fourth line button (G) together to clear the memory.
    The game will automatically enter the Machine Options menu where you can change the QCOM poll address, serial number, denomination and bill acceptor settings.
    The poll address must be a number from 1 to 255. The serial number cannot be zero.
    Bill acceptor options should remain disabled until implemented in MAME, otherwise the machine will give a bill acceptor stacker error.
    Press Reserve (A) to save changes, and key out (F2) to exit the menu.
    Note 1: Once these settings have been saved, they cannot be changed unlike games from other regions as the Machine Options menu is hidden.
    Note 2: As the QCOM protocol is not yet emulated, the games are not playable since they automatically disable themselves after timing out.


    Queensland games, newer types:
    dolphntrql, indrema5ql, qnilefl, qnileql, reelrockql

    Audit key in, press Collect (Q) and the fourth line button (G) together to clear the memory.
    The game will automatically enter the Machine Options menu where you can change the serial number (it cannot be zero) and enable/disable the bill acceptor.
    Bill acceptor options should remain disabled until implemented in MAME, otherwise the machine will give a bill acceptor stacker error.
    Press Collect (Q) to save changes, and key out (F2) to exit the menu.
    Note 1: Once these settings have been saved, they cannot be changed unlike games from other regions as the Machine Options menu is hidden.
    Note 2: As the QCOM protocol is not yet emulated, the games are not playable since they automatically disable themselves after timing out.


    Casino games using the New Zealand 0700474V base EPROMs:
    Currently, this is limited to goldpyrb, jungjuic and penpir2 (and also qnilebr if you swap the base EPROMs). The games are universal (all regions), however the base is region coded.

    Step 1: Audit key in, press Service (A) and the fourth line button (G) together to clear the memory.

    Step 2: Enter Options Setup -> Machine Options and follow the same instructions as per the NSW/ACT games. The game should already have some default settings, but they can still be changed.
    Note that like Queensland games, this casino base requires QCOM thus the games are not yet playable with these base EPROMs.


    Casino games using the Brazilian 0301718V base EPROMs:
    Currently this is limited to qnilebr (and the above three casino games if you swap the base EPROMs).

    This is similar to the NSW/ACT games, however upon keying in (F2), it requires a four-digit setup code before it can be initialized.
    By default, this number is 4856. After the game has accepted this code, press Service (A) and Play 7 Lines (G) at the same time to clear the memory.
    The game will then tell you to press Bet 1 (W) to continue.

    You will now be asked to change the four digit code for the Audit menu. Do not forget this number otherwise you will not be able to enter the menu.
    When the code has been inputted, you will have to repeat the code and then confirm with the Bet 1 button (W).
    Two other similar menus will appear, requiring similar new codes, which are to be inputted and confirmed in the same way.
    After confirming the third code, the machine will tell you to key out (F2). The game should now be ready to accept credits.

    Note: To change settings, key in (F2) and enter the Operator Setup (Opcoes de Configuracao) -> Machine Options (Itens Gerais) as per the NSW/ACT games.
    Once the required settings have been changed, open the logic door (L), press Bet 1 (W) to save the settings, close the logic door and key out.

    Running non-Brazilian casino games with the Brazil base may cause minor text errors due to the games not having accented characters in the fonts,
    however they are still fully playable.

    *************************************************************************************************************

    USA platform, games without set chips:

    Step 1: Set up the DIP switches to the desired values.

    DIP Switch Bank 1 (DSW1) is comprised of coin/credit values.

    The first four switches control the coin (token) type:
    0000 = $0.05 (5 cents/nickels)
    1000 = $0.10 (10 cents/dimes)
    0100 = $0.25 (25 cents/quarters)
    1100 = $0.50 (50 cents/half dollars)
    0010 = $1.00 (dollar coins)
    1010 = $5.00
    0110 = $10.00
    1110 = $50.00
    xxx1 = $100.00 (any value with the fourth switch set is $100.00)

    The fifth through eighth switches control the machine's denomination:
    0000 = $0.01 (e.g. a 1 cent machine)
    1000 = $0.02
    0100 = $0.05
    1100 = $0.10
    0010 = $0.25
    1010 = $0.50
    0110 = $1.00
    1110 = $5.00
    0001 = $10.00
    1001 = $50.00
    x101 = $100.00
    xx11 = $100.00

    Note that not all combinations are valid e.g. they must be evenly divisible and a token cannot be less than the machine's denomination.

    DIP switch settings are normally only read once by the machine on start-up. To visibly change DIP settings you need to reset the machine (F3) or perform a memory clear from the main menu (see Step 2 below).

    Each machine has its own DIP switch options concerning DIP Switch Bank 2 (DSW2), however the gamble feature is universally the first switch.
    Per-game DIP switch information is available in the Operator Setup -> Dip Switch Settings menu.
    Once a machine has been set up, the DIP switches are ignored from then on, even if they have been changed.

    Step 2: Audit key in, enter Operator Setup -> Machine Options and set up the machine to the applicable standards.
    To save the settings, open the Logic Door (L), press the Bet 1 Credit button (S), close the Logic Door and exit the menu (or press F2 for a shortcut to the main menu).

    Step 3: The machine will now need a memory reset. From the main menu, open the Main Door (M) and press Cashout (A) and the fourth line button (T) together to clear the memory.
    After closing the Main Door (M) and keying out (F2), the menu should close and the game should be ready to coin up.

    Hint: If you need to change the DIP switches after a game has been set up, use a set chip to clear the memory (press Cashout+Service (A+S) once the set chip has loaded, and then go back to game mode).
    To switch between game mode and set chips, you need to reset the machine (F3) as it is technically swapping the ROMs on the board.
    Performing a memory reset will not erase the machine options, but it will cause a memory error which will allow you to set up anything else.
    Using the Clear All Memory option from the set chip menu (assuming 4.04.xx) will erase everything however.

    *************************************************************************************************************

    USA platform, games requiring set chips:

    Step 1: Take note of Bet/Line setup. If an invalid bet/line combination is used, the machine cannot perform a memory clear or be set up so it is important to check.

    Key in (F2), Enter Operator Setup (T), Bet/Line Denom Settings to view allowable bet/line options
    For example, qnilece only allows 20 lines with 5, 10, 25 or 50 credit multipliers, all other options are invalid.
    The default (and usually invalid) setting is 1 line with a 3 credit multiplier, so it has to be changed in the set chip.

    Step 2: Take note if the set chip version. As with having a wrong bet/line setting, the machine cannot be set up if the wrong set chip version is used.

    Assuming you are still in the audit menu (press F2 anyway, it will be a shortcut to the main menu if a game hasn't been set up yet), enter Machine Identification
    Note: This may cause the game to reboot so pause the emulation as soon as you enter the menu if you need more time to read the set chip version number. Remember to un-pause emulation afterward!

    Using qnilece as an example, this particular game requires set chip version is 4.04.xx. This means that any set chip which is compatible with 4.04.xx can be used with this game (the "xx" digits don't matter).

    Step 3: Now that we know the correct bet/line values and the set chip version it is time to swap the U7 and U11 game chips for the set chips.

    To do this in MAME, press Tab to open the menu and enter Machine Configuration and select an appropriate set chip with the left/right keys.
    It is best to use 4.04.09 for all games as it is the latest version supported in MAME; set chips are backward compatible with previous versions so they can also be used to set up older games (for example, cashchamu requires 4.00.xx).
    You should now be in the set chip menu (if not, hit F3).

    Press Service and Cashout together (Q+A) to reset the SRAM and enter the Version Menu.
    Service and Cashout are your up/down cursor keys in the set chip menu, for qnilece we need to set the machine to 4.04.xx so press Q or A until a suitable version appears and press F to confirm selection.
    Press F again in the Game Type Setup Menu as this game is a video slot (as opposed to video poker).

    You will now be in the main menu. Press G to enter a sub-menu and F to exit a sub-menu.
    To change the options, press E or R to move the selection back or forward respectively.

    Step 3a: Jurisdictional Options

    Enter the Jurisdictional Options Menu to set up the region and various options.
    Usually, only the jurisdiction and/or gamble feature need to be toggled (the latter is required in order to enable the double up option). Bill acceptors are not emulated as of 0.198.

    Jurisdiction: This option sets the various US regions or Bolivia, Peru/Sask, Puerto Rico or Uruguay.
    Each region has its own gambling laws and restrictions which is why there are so many options.
    The set chip region with the least restrictions is actually Uruguay rather than a US state, as US slot machines are not allowed to store more than $1200.00 worth of credits and/or wins, and wins in excess of the limit must be hand paid.

    Cancel Residual Credit: Soft Lockup or Hard Lockup. Residual credits are credits below the minimum value which cannot be cashed out as coins or otherwise.
    A soft lockup allows the player to either call an attendant for a hand pay or continue playing, while a hard lockup requires a hand pay of the remaining credits.

    Gamble Feature: Toggle between Disabled or Enabled.
    The gamble feature is not available in the following regions: Arizona, California, Colorado, North Dakota, South Dakota, Wisconsin and Puerto Rico.

    Bill Acceptor Protocol: (To be written once emulated)
    EFT/Bonus Transfer: Cashless/ticket acceptance options, not required in MAME.

    The following items are hard coded and cannot be changed:
    Maximum Bet Limit, Max Bet Coin Reject, Tokenization, Cashout Device, Maximum Percentage.
    The only setting of note is Tokenization. If this is enabled, one coin (token) can represent multiple credits, otherwise if disabled, one coin (token) is one credit.

    Step 3b: Money Setup and Payout Setup

    This is where the coin/credit (denomination) values can be changed. Note that some settings cannot be used with some games, or if the coin cannot be evenly divided into credits or is worth less than one credit.
    By default, the machine is set to 5 cents buys 1 credit, which basically means a 1 cent (penny) slot which accepts nickels.
    The higher the value of one coin, the more credits can be inserted at a time (unless one coin is equal to one credit).
    The higher the value of one credit, the more money a win is worth e.g. 1000 credits on a penny slot is ten dollars whereas 1000 credits on a nickel slot is fifty dollars.
    If tokenization is disabled, the coin and credit values must be identical.
    Max Credit Limit is the maximum amount of credits allowed in the machine. While the menu allows you to select up to $999999.99 as an option, the maximum possible value is $99999.99
    Tax Limit cannot be changed, it is hard coded to the region ($1200.00 for US setups, $999999.99 for non-US setups).

    Payout Setup Menu: This menu is not required in MAME.

    Step 3c: Playline Setup Menu
    Remember how qnilece only allowed 20 lines, with bets of 5, 10, 25 and 50 credits? It's time to tell the set chip that this game is not a three credit single line game.
    Of course, the higher the maximum bet, the larger the wins are at the cost of how much money you put through the machine.
    Most games also allow the button panel to be reversed, with the bet buttons on the top row and the playline buttons on the bottom, similar to non-US machines. This is toggled with G.
    Note that the intermediate bet values shown on the button display may not correspond perfectly with the actual bet values in the game itself; this information is not actually stored in the set chip and is up to the game's programming.

    Step 3d: Miscellaneous Options Menu
    Variation Number is how loose or tight you want the machine to be. By default, it is variation 99, which is usually around 87%, give or take 1% depending on the machine.
    Variation 99: 87%
    Variation 1: 90%
    Variation 2: 92%
    Variation 3: 94%
    Variation 4: 97%
    Variation 5: 82%
    Usually, variation numbers beyond 4 are not supported, aside from variation 5 for Hyperlink games. Variation 6 identifies as a Hyperlink setting but no games support this value.
    To enable the Hyperlink feature, variation 5 must be selected. This variation usually has a significantly lower payout percentage than the standard settings, with the progressive jackpots making up the remaining payouts.
    Note: Not all percentage variations can be selected, and Hyperlink features (e.g. Cash Express) cannot be enabled on games which do not have them.
    Also note: As the external link system is not emulated in MAME, it is recommended to only use variations 99 and/or 1-4 (if applicable) as you cannot win any Hyperlink jackpot credits without the link.

    Hyperlink Turnover is the value to set when enabling the jackpot bonus. The higher the value, the harder the jackpot feature is to trigger, although the progressive jackpot will grow larger in the process.
    Note that this setting is dependent on the machine's credit value (denomination) and bet settings, and usually only one or two settings will be valid.

    Play Bet Button Functionality: Continuous allows the play/bet button to be held down to continue playing automatically, while Toggle forces the button to be pressed for every paid game and will not automatically spin if held.

    Step 3e: If all of the above have been done, open the Logic Door (L) and press press G to save the changes. Optionally, you can close the Logic Door at this point.
    It is time to save everything and swap the set chips back to the game EPROMs.
    If all is OK, you should now see the line markers on the game screen and the coin/credit value in the top right corner.
    If anything has gone wrong after going back to the game EPROMs and you have a jurisdictional error, the setup error(s) should be visible in the game's audit menu under Machine Lockups.

    Step 4: Setting up the machine.
    If there are no jurisdictional errors after going back to the game EPROMs, it is now time to set up the machine itself.
    To reset the memory (EEPROM/static RAM), key in (F2), open the main door (M) and press Cashout and Service together (Q+A). This will reset the EEPROM so you can set up the machine for good.

    Enter the Operator Setup / Game Machine Options menu.
    Usually, most the default settings are fine, but the machine won't let you save until at least one item has been changed, even if it is reverted.
    You might want to change the jackpot settings, as by default it is only 200 credits and will force a hand pay for wins over that amount. The higher the better of course, if you do not want constant hand pays.

    To save settings, open the Logic Door (L) and go to Save Machine Options and hit (T) and close the Logic Door. The Logic Door must be open otherwise all changes will be lost if you exit the menu.

    Once saved, go back to the main menu (or hit F2 for a shortcut) and reset the static RAM again and close any open doors such as the Main Door (M).
    If all has gone well, there should be no more errors and you should be able to exit the audit menu (F2) and coin up!
    If not, check the Current Lockup menu and it should show you any remaining errors or any doors that may still be open.

*****************************************************************************************************************

    TODO (MK5 specific):
    - Fix remaining errors
    - Layouts for various configurations
    - Bill acceptor
    - Serial printer
    - Default NVRAM

    code DASMing of POST (adonis):
    - bp 0x3400224:
      checks work RAM [0x87000], if bit 0 active high then all tests are skipped (presumably for debugging), otherwise check stuff;
        - bp 0x3400230: EPROM checksum branch test
        - bp 0x3400258: DRAM Check branch test
        - bp 0x3400280: CPU Check branch test
            bp 0x340027c: checks IRQ status A and FIQ status bit 7 (force IRQ flag)
            - R0 == 0: CPU Check OK
            - R0 == 1: IRQ status A force IRQ flag check failed
            - R0 == 2: FIQ status force IRQ flag check failed
            - R0 == 3: Internal Latch check 0x3250050 == 0xf5
        - bp 0x34002a8: SRAM Check branch test (I2C)
            - basically writes to the I2C clock/data then read-backs it
        - bp 0x34002d0: 2KHz Timer branch test
            bp 0x34002cc: it does various test with GO command reads (that are undefined on plain AA) and
                          IRQA status bit 0, that's "printer busy" on original AA but here it have a completely
                          different meaning.
        - bp 0x34002f8: DRAM emulator branch tests
            bp 0x34002f4:
            - R0 == 0 "DRAM emulator found"
            - R0 == 1 "DRAM emulator found"
            - R0 == 3 "DRAM emulator not found - Error"
            - R0 == 4 "DRAM emulator found instead of DRAM - Error"
            - R0 == x "Undefined error in DRAM emulator area"
            It r/w RAM location 0 and it expects to NOT read-back value written.

    goldpyr: checks if a "keyboard IRQ" fires (IRQ status B bit 6), it seems a serial port with data on it,
              returns an External Video Crystal Error (bp 3400278)

    dimtouch:
        bp 3400640: checks 2MByte DRAM
            - writes from 0x1000 to 0x100000, with 0x400 bytes index increment and 0xfb data increment
            - writes from 0x100000 to 0x200000, with 0x400 bytes index increment and 0xfb data increment
            - bp 3400720 checks if the aforementioned checks are ok (currently fails at the very first work RAM check
              at 0x1000, it returns the value that actually should be at 0x141000)
        bp 340064c: if R0 == 0 2MB DRAM is ok, otherwise there's an error

    set chip (BIOS):
        same as goldpyr (serial + ext video crystal check)
        bp 3400110: External Video Crystal test

*****************************************************************************************************************

  MKV S2 Mainboard, PCB Layout:

  +--------------------------------------------------------------------------------------------------------+
  |   |    96-pin male connector     |  |    96-pin male connector     |  |    96-pin male connector     | |
  |   +------------------------------+  +------------------------------+  +------------------------------+ |
  |            +---+       +--+                                                          +---+ +---+ +---+ |
  +-+          |VR1|       |  |U89              +------+        +------+                 |AA | |AB | |AC | |
  | |          |   |       |  |                 |AMP   |        |U35   |                 +---+ +---+ +---+ |
  |S|          |   |       |  |                 +------+        +------+                 +---+ +---+ +---+ |
  |I|          +---+       +--+ +--+                                                     |U46| |U21| |U66| |
  |M|                           |  |U52                  ARISTOCRAT                      +---+ +---+ +---+ |
  |M|                    +----+ |  |                     MKV S2 MAINBOARD                   +------------+ |
  | |    +---------+     |U47 | |  |                     PCB 0801-410091                    | 3V BATTERY | |
  |S|    |U72      |     +----+ +--+                     ASSY 2501-410389                   |            | |
  |O|    |         |       +-----+                       ISSUE A01                          +------------+ |
  |C|    |         |       |U23  |                                                                  +----+ |
  |K|    |         |       |     |                                                                  |U53 | |
  |E|    +---------+       +-----+       +------------+                                             +----+ |
  |T|                                    |U85         |    +----+ +----+ +--+ +--+ +----+ +----+ +--+ +--+ |
  | |    +---------+                     |            |    |U58 | |U54 | |U | |U | |U59 | |U61 | |U | |U | |
  | |    |U71      |                     |    CPU     |    +----+ +----+ |1 | |4 | +----+ +----+ |1 | |4 | |
  | |    |         |                     |            |           +----+ |4 | |8 |  +------+     |5 | |9 | |
  | |    |         |  +-----+            |            |           |U56 | |9 | |  |  |U36   |     |2 | |  | |
  | |    |         |  |U65  |            |            |           +----+ +--+ +--+  |      |     +--+ +--+ |
  | |    +---------+  |     |            +------------+                             +------+               |
  | |                 +-----+     +-----+ +---+                                                            |
  +-+    +---+                    |U73  | |X2 |                    +----------------+   +----------------+ |
  |      |U26|                    |     | +---+   +---+            |U14             |   |U10             | |
  |      +---+                    +-----+         |U50|            |                |   |                | |
  |      |U27|                       +-----+      +---+            +----------------+   +----------------+ |
  |      +---+                       |U5   |      |U40|            |U13             |   |U9              | |
  |                                  |     |      +---+            |                |   |                | |
  |                                  +-----+      |U41|            +----------------+   +----------------+ |
  |                                               +---+            |U12             |   |U8              | |
  |          +---+                                                 |                |   |                | |
  |          |VR2|                         +-----+         +-----+ +----------------+   +----------------+ |
  |          |   |                         |U24  |         |U22  | |U11             |   |U7              | |
  |          |   |                         |     |         |     | |                |   |                | |
  |          |   |                         +-----+         +-----+ +----------------+   +----------------+ |
  |          +---+                                            +----------------------------------+         |
  |                                                           |     96-pin female connector      |         |
  +--------------------------------------------------------------------------------------------------------+

  U5: 48 MHz crystal (unpopulated from factory).

  U7:  27C4096 ROM socket (bank 0).
  U8:  27C4096 ROM socket (bank 1).
  U9:  27C4096 ROM socket (bank 2).
  U10: 27C4096 ROM socket (bank 3).

  U11: 27C4096 ROM socket (bank 0).
  U12: 27C4096 ROM socket (bank 1).
  U13: 27C4096 ROM socket (bank 2).
  U14: 27C4096 ROM socket (bank 3).

  U21: NEC D43256BGU-70LL (32k x 8bit CMOS Static RAM).
  U22: LATTICE GAL20V8B-15LJ (High Performance E2CMOS PLD Generic Array Logic, 28-Lead PLCC).
  U23: LATTICE GAL16V8D-25LJ (High Performance E2CMOS PLD Generic Array Logic, 20-Lead PLCC).
  U24: LATTICE GAL16V8D-25LJ (High Performance E2CMOS PLD Generic Array Logic, 20-Lead PLCC).
  U26: SGS THOMSON ST93C46 (1K (64 x 16 or 128 x 8) Serial EEPROM).
  U27: SGS THOMSON ST93C46 (1K (64 x 16 or 128 x 8) Serial EEPROM).

  U35: PHILIPS 74HC273.
  U36: LATTICE GAL20V8B-15LJ (High Performance E2CMOS PLD Generic Array Logic, 28-Lead PLCC).
  U40: Dallas Semiconductor DS1202S (Serial Timekeeping Chip).
  U41: Maxim Integrated MAX705CSA (MPU Supervisory Circuits).
  U46: NEC D43256BGU-70LL (32k x 8bit CMOS Static RAM).
  U47: Maxim Integrated MAX202CWE (RS-232 Interface IC).
  U48: ISSI IS41C16257-60K (256K x 16bit (4-MBIT) Dynamic RAM With Fast Page Mode).
  U49: ISSI IS41C16257-60K (256K x 16bit (4-MBIT) Dynamic RAM With Fast Page Mode).
  U50: Dallas Semiconductor DS1620 (Digital Thermometer and Thermostat).
  U52: Allegro MicroSystems UDN2543B (Protected quad power driver).
  U53: SGS THOMSON 74HC244 (Octal buffer/line driver, 3-state).
  U54: Motorola AC244 (Octal Buffer/Line Driver with 3-State Outputs).
  U56: SGS THOMSON 74HC244 (Octal buffer/line driver, 3-state).
  U58: Motorola AC244 (Octal Buffer/Line Driver with 3-State Outputs).
  U59: Motorola AC244 (Octal Buffer/Line Driver with 3-State Outputs).
  U61: Motorola AC244 (Octal Buffer/Line Driver with 3-State Outputs).
  U65: LATTICE GAL20V8B-15LJ (High Performance E2CMOS PLD Generic Array Logic, 28-Lead PLCC).
  U66: NEC D43256BGU-70LL (32k x 8bit CMOS Static RAM).
  U71: Texas Instruments TL16C452FN (UART Interface IC Dual UART w/Prl Port & w/o FIFO).
  U72: Texas Instruments TL16C452FN (UART Interface IC Dual UART w/Prl Port & w/o FIFO).
  U73: CX0826 72 MHz crystal.
  U85: ARM250: Computer system on a chip. ARM 32bit RISC processor with memory, video, and I/O controllers.
  U89: Allegro MicroSystems UDN2543B (Protected quad power driver).
  U149: ISSI IS41C16257-60K (256K x 16bit (4-MBIT) Dynamic RAM With Fast Page Mode).
  U152: ISSI IS41C16257-60K (256K x 16bit (4-MBIT) Dynamic RAM With Fast Page Mode).

  AA:  SGS THOMSON 74HC244 (Octal buffer/line driver, 3-state).
  AB:  SGS THOMSON 74HC244 (Octal buffer/line driver, 3-state).
  AC:  PHILIPS 74HC245D (Octal bus transceiver, 3-state).

  AMP: TDA 2006 (12W Audio Amplifier).

  VR1: Motorola 7805 (3-Terminal 1A Positive Voltage Regulator).
  VR2: SGS THOMSON L4975A (5A stepdown monolithic power switching regulator at 5.1V-40V).

  X2:  Unpopulated crystal (from factory).

  The 96-pin female connector at the bottom of the ROM banks is intended for a sub board
  with two ROM sockets, that once plugged switch the ROM bank 0 with the sub board bank.
  Just to place the clear chips without removing the U7 & U11 EPROMS.

*****************************************************************************************************************/

#include "emu.h"
#include "includes/archimds.h"
#include "cpu/arm/arm.h"
#include "machine/ds1302.h"
#include "machine/watchdog.h"
#include "machine/eepromser.h"
#include "machine/microtch.h"
#include "machine/input_merger.h"
#include "machine/nvram.h"
#include "machine/ins8250.h"
#include "machine/ticket.h"
#include "sound/volt_reg.h"
#include "speaker.h"

// Non-US button layouts    Bet buttons       Lines Gamble     Other
#include "aristmk5.lh"   // 1, 2, 3, 5, 10    20    suits      Take Win/Start Feature
#include "adonisce.lh"   // 1, 2, 3, 5, 10    20    suits      TW/SF, Cash Express buttons
#include "baddog.lh"     // Video Poker
#include "cashcatnz.lh"  // 1, 2, 3, 4, 5     9     suits      TW/SF, 7L or Autoplay
#include "cashcham.lh"   // 1, 5, 10, 20, 25  20    suits
#include "cashchama.lh"  // 1, 2, 3, 4, 5     20    suits
#include "cashchamnz.lh" // 1, 2, 3, 4, 5     20    suits      15L or Autoplay
#include "checkma5.lh"   // multiple configs  1/3   suits      TW/SF
#include "chickna5v.lh"  // 1, 5, 10, 25, 50  9     suits
#include "coralrc2.lh"   // 1, 2, 3, 5, 25    20    suits
#include "dimtouch.lh"   // 1, 2, 3, 5, 10    9     suits      TW/SF (touch-based gamble feature)
#include "dolphntrb.lh"  // 1, 2, 3, 5, 10    9     suits      TW/SF
#include "dreamwv.lh"    // 1, 2, 5, 10, 20   9     suits      TW/SF (touch-based gamble feature)
#include "dynajack.lh"   // multiple configs  9/20  suits      TW/SF
#include "fortellr.lh"   // multiple configs  9/20  suits      TW/SF (different bets to dynajack)
#include "geisha.lh"     // 1, 2, 3, 5, 12    20    suits      TW/SF, 15L or Autoplay
#include "genmagi.lh"    // 1, 2, 3, 5, 25    20    suits      TW/SF (touch-based gamble feature)
#include "goldenra.lh"   // 1, 2, 5, 25, 50   20    suits      TW/SF
#include "goldpyrb.lh"   // 1, 2, 3, 5, 10    9     suits      TW/SF, Service
#include "incasun.lh"    // 1, 2, 3, 5, 25    20    suits      TW/SF
#include "incasunnz.lh"  // 1, 2, 3, 4, 5     20    suits      TW/SF, 15L or Autoplay
#include "indrema5.lh"   // 1, 2, 5, 10, 20   243   suits      TW/SF
#include "jungjuic.lh"   // 1, 2, 3, 4, 5     9     red/black  Service
#include "kgalah.lh"     // 1, 2, 3, 5, 10    20    suits
#include "kgbirda5.lh"   // 1, 2, 3, 5, 10    5     red/black
#include "locoloota.lh"  // 1, 2, 5, 10, 20   9     suits
#include "marmagic.lh"   // multiple configs  9/20  suits      TW/SF (different bets to dynajack/fortellr)
#include "montree.lh"    // 1, 2, 3, 5, 12    20    suits      15L or Autoplay
#include "mountmon.lh"   // 1, 5, 10, 25, 50  20    red/black
#include "multidrw.lh"   // Video Poker (different to baddog)
#include "mystgard.lh"   // 1, 2, 3, 4, 5     20    red/black
#include "one4all.lh"    // 1, 2, 3, 5, 6     20    suits      TW/SF, 15L or Autoplay
#include "orchidms.lh"   // 1, 5, 10, 25, 50  10    suits
#include "orchidmsnz.lh" // 1, 2, 5, 15, 25   10    suits      TW/SF, 8L or Autoplay
#include "pantmag.lh"    // 1, 2, 3, 5, 10    9/20  suits
#include "pantmaga.lh"   // 1, 2              5     suits
#include "penpir2.lh"    // 1, 2, 3, 5, 10    20    suits      Service
#include "petshop.lh"    // 1, 2, 3, 5, 10    20    suits      Different layout to other games
#include "przfight.lh"   // 1, 2, 3           3     red/black
#include "qnile.lh"      // 1, 5, 10, 20, 25  20    suits      TW/SF
#include "qnilec.lh"     // 1, 2, 5, 10, 20   9     suits      TW/SF
#include "qniled.lh"     // 1, 2, 3           3     suits      TW/SF
#include "qnilenl.lh"    // 1, 2, 3, 5, 10    9     red/black  TW/SF, Service
#include "qtbird.lh"     // 1, 2, 3, 4, 5     9     red/black
#include "reelrock.lh"   // 1, 2, 3, 5, 8     243   suits
#include "retrsam.lh"    // 1, 2, 3           3     odds
#include "retrsamb.lh"   // 1, 2, 3, 5, 10    9     odds
#include "sbuk2.lh"      // 1, 2, 3, 4, 5     1     red/black  Single line game
#include "sbuk3.lh"      // 1, 2, 3           3     odds       TW/SF
#include "snowcat.lh"    // 1, 2, 3, 5, 10    9     suits
#include "swhr2.lh"      // 1, 2, 3, 5, 10    9     red/black
#include "toutangonl.lh" // 1, 2, 3, 5, 10    9     red/black  Service
#include "tritreat.lh"   // multiple configs  9/20  suits      TW/SF (90 and 500 credit play)
#include "trstrove.lh"   // 1, 2, 5, 10, 25   20    suits      TW/SF
#include "venicea5.lh"   // multiple configs  9/20  suits      TW/SF (different bets to dynajack/fortellr/marmagic)
#include "wamazon.lh"    // 1, 2, 3           3     suits      Play Feature Game
#include "wamazona.lh"   // 1, 2, 3, 5, 10    1     suits      Single line game
#include "wcougar.lh"    // 1, 2, 5, 10, 20   9     red/black
#include "wikwin.lh"     // 1, 2, 3, 4        243   suits      Max Bet button
#include "wildbill.lh"   // 1, 2, 3           3     suits
#include "wizways.lh"    // 1, 2, 5, 10, 20   243   suits
#include "wtiger.lh"     // 1, 2, 3, 5, 10    20    suits      Classic Buy Feature
#include "yukongl5.lh"   // multiple configs  20    suits      TW/SF

// US button layouts             Lines  Bet buttons     Max bet(s)                    Other
#include "adonisu.lh"         // 20     multiple        100, 200, 500, 1000
#include "aristmk5_us.lh"     // 9      1, 2, 3, 5, 10  90
#include "aristmk5_us_200.lh" // 20     1, 2, 3, 5, 10  200
#include "bparty.lh"          // 20     multiple        60, 100, 200, 400, 500, 1000
#include "cuckoou.lh"         // 9      multiple        27, 45, 90, 180, 225, 450
#include "cashchamu.lh"       // 20     multiple        60, 100, 200, 400, 500, 1000  Different layout on 3 credit setup
#include "dolphntrceb.lh"     // 20     multiple        60, 100, 200, 400, 500, 1000  Different bets to bparty, cashchamu and pengpuck
#include "magimaska.lh"       // 9      multiple        45, 90, 180
#include "marmagicu.lh"       // 9      multiple        45, 90, 180                   Same bet buttons as magimaska but with Start Feature button
#include "pengpayce.lh"       // 9      multiple        27, 45, 90, 180, 225, 450     Different layout to cuckoou
#include "pengpuck.lh"        // 20     multiple        60, 100, 200, 400, 500, 1000  Slightly different layout to dolphntrceb
#include "qnilecea.lh"        // 9      multiple        27, 45, 90, 180, 225, 450     Same bet buttons as pengpayce but with Start Feature button
#include "swhr2u.lh"          // 5      1, 2, 3, 4, 5   25
#include "wnpost.lh"          // 5      1, 2, 3, 5, 10  50

#define MASTER_CLOCK        72_MHz_XTAL      /* confirmed */

class aristmk5_state : public archimedes_state
{
public:
	aristmk5_state(const machine_config &mconfig, device_type type, const char *tag)
		: archimedes_state(mconfig, type, tag)
		, m_hopper(*this, "hopper")
		, m_eeprom(*this, "eeprom%d", 0)
		, m_rtc(*this, "rtc")
		, m_nvram(*this, "nvram")
		, m_sram(*this, "sram")
		, m_p1(*this, "P1")
		, m_p2(*this, "P2")
		, m_extra_ports(*this, "EXTRA")
		, m_lamps(*this, "lamp%u", 0U)
	 { }

	static constexpr feature_type imperfect_features() { return feature::GRAPHICS | feature::SOUND | feature::TIMING; }

	void aristmk5(machine_config &config);
	void aristmk5_touch(machine_config &config);
	void aristmk5_usa(machine_config &config);
	void aristmk5_usa_touch(machine_config &config);

	void init_aristmk5();

	INPUT_CHANGED_MEMBER(coin_start);
	CUSTOM_INPUT_MEMBER(coin_r);
	CUSTOM_INPUT_MEMBER(coin_usa_r);
	CUSTOM_INPUT_MEMBER(hopper_r);

private:
	DECLARE_WRITE32_MEMBER(Ns5w48);
	DECLARE_READ32_MEMBER(Ns5x58);
	DECLARE_READ32_MEMBER(Ns5r50);
	DECLARE_WRITE8_MEMBER(sram_banksel_w);
	DECLARE_WRITE8_MEMBER(eeprom_w);
	DECLARE_WRITE8_MEMBER(eeprom_usa_w);
	DECLARE_WRITE8_MEMBER(rtc_w);
	DECLARE_WRITE8_MEMBER(rtc_usa_w);
	DECLARE_WRITE8_MEMBER(hopper_w);
	DECLARE_READ8_MEMBER(eeprom_r);
	DECLARE_READ8_MEMBER(ldor_r);
	DECLARE_WRITE8_MEMBER(ldor_clk_w);
	DECLARE_WRITE8_MEMBER(buttons_lamps_w);
	DECLARE_WRITE8_MEMBER(other_lamps_w);
	DECLARE_WRITE8_MEMBER(bill_acceptor_lamps_w);
	DECLARE_READ8_MEMBER(sram_r);
	DECLARE_WRITE8_MEMBER(sram_w);
	DECLARE_WRITE8_MEMBER(spi_mux_w);
	DECLARE_WRITE8_MEMBER(spi_data_w);
	DECLARE_READ8_MEMBER(spi_int_ack_r);
	DECLARE_WRITE8_MEMBER(spi_int_ack_w);
	DECLARE_READ8_MEMBER(spi_data_r);
	DECLARE_WRITE_LINE_MEMBER(uart_irq_callback);

	virtual void machine_start() override;
	virtual void machine_reset() override;
	TIMER_CALLBACK_MEMBER(mk5_VSYNC_callback);
	TIMER_CALLBACK_MEMBER(mk5_2KHz_callback);
	TIMER_CALLBACK_MEMBER(spi_timer);

	void aristmk5_drame_map(address_map &map);
	void aristmk5_map(address_map &map);
	void aristmk5_usa_map(address_map &map);

	required_device<ticket_dispenser_device> m_hopper;

	uint8_t         m_hopper_test;
	uint64_t        m_coin_start_cycles;
	uint8_t         m_coin_div;

	required_device_array<eeprom_serial_93cxx_device, 2> m_eeprom;
	required_device<ds1302_device> m_rtc;
	required_device<nvram_device> m_nvram;
	required_memory_region m_sram;
	required_ioport m_p1;
	required_ioport m_p2;
	required_ioport m_extra_ports;

	output_finder<64> m_lamps;

	emu_timer *     m_mk5_2KHz_timer;
	emu_timer *     m_mk5_VSYNC_timer;
	emu_timer *     m_spi_timer;
	uint8_t         m_sram_bank;
	uint8_t         m_ldor_shift_reg;
	uint8_t         m_spi_mux;
	uint8_t         m_spi_latch;
	uint8_t         m_spi_bits;
	uint32_t        m_spi_data[8];
};


WRITE8_MEMBER(aristmk5_state::spi_mux_w)
{
	uint8_t spi_mux = (data >> 4) & 7;

	if (spi_mux == m_spi_mux)
		return;

	m_spi_mux = spi_mux;

	switch (m_spi_mux)
	{
	case 0: // Test
	case 3: // not used
		break;

	case 1: // Top box lamps
		break;

	case 2: // Mechanical meters
		for(int i = 0; i < 4; i++)
		{
			m_lamps[32+i] = BIT(m_spi_data[m_spi_mux], 1 + i); // Tower Lamps
		}
		break;

	case 4: // Door inputs
		m_spi_data[m_spi_mux] = m_p1->read();
		break;

	case 5: // Door outputs
		for(int i = 0; i < 32; i++)
		{
			m_lamps[i] = BIT(m_spi_data[m_spi_mux], i);
		}
		break;

	case 6: // Main board slow I/O
		m_spi_data[m_spi_mux] = m_p2->read() & ~((data  & 0x80) ? 0 : 0x100);
		break;

	case 7: // Main board security registers
		break;
	}
}

WRITE8_MEMBER(aristmk5_state::spi_data_w)
{
	m_spi_latch = data;
	m_spi_bits = 0;

	// start the SPI clock
	m_spi_timer->adjust(attotime::from_hz(MASTER_CLOCK / 9 / 512 / 2), 0, attotime::from_hz(MASTER_CLOCK / 9 / 512 / 2));
}

READ8_MEMBER(aristmk5_state::spi_data_r)
{
	return m_spi_latch;
}

READ8_MEMBER(aristmk5_state::spi_int_ack_r)
{
	archimedes_clear_irq_b(0x08);
	return 0;
}

WRITE8_MEMBER(aristmk5_state::spi_int_ack_w)
{
	archimedes_clear_irq_b(0x08);
}

TIMER_CALLBACK_MEMBER(aristmk5_state::spi_timer)
{
	if (m_spi_mux == 0 || m_spi_mux == 3)
	{
		m_spi_latch = (((m_spi_latch & 1) << 7) ^ 0x80) | ((m_spi_latch >> 1) & 0x7f);
	}
	else
	{
		static int mux_bits[8] = { 0, 16, 16, 0, 24, 32, 24, 8 };

		uint32_t mux_mask = ((uint32_t)1 << (mux_bits[m_spi_mux] - 1)) - 1;
		uint32_t spi_in_bit = m_spi_data[m_spi_mux] & 1;
		uint32_t spi_out_bit = m_spi_latch & 1;

		m_spi_data[m_spi_mux] = (spi_out_bit << (mux_bits[m_spi_mux] - 1)) | ((m_spi_data[m_spi_mux] >> 1) & mux_mask);
		m_spi_latch = (spi_in_bit << 7) | ((m_spi_latch >> 1) & 0x7f);
	}

	// SPI interrupt
	if (++m_spi_bits == 8)
	{
		m_spi_timer->adjust(attotime::never);
		archimedes_request_irq_b(0x08);
	}
}

WRITE_LINE_MEMBER(aristmk5_state::uart_irq_callback)
{
	if (state)
		archimedes_request_irq_b(0x20);
	else
		archimedes_clear_irq_b(0x20);
}

TIMER_CALLBACK_MEMBER(aristmk5_state::mk5_VSYNC_callback)
{
	archimedes_request_irq_a(0x08); //turn vsync bit on
	m_mk5_VSYNC_timer->adjust(attotime::never);
}

READ8_MEMBER(aristmk5_state::sram_r)
{
	return m_sram->base()[(m_sram_bank << 14) | (offset & 0x3fff)];
}

WRITE8_MEMBER(aristmk5_state::sram_w)
{
	m_sram->base()[(m_sram_bank << 14) | (offset & 0x3fff)] = data;
}

WRITE32_MEMBER(aristmk5_state::Ns5w48)
{
	/*
	There is one writeable register which is written with the Ns5w48 strobe. It contains four bits which are
	taken from bits 16 to 19 of the word being written. The register is cleared whenever the chip is reset. The
	register controls part of the video system. Bit 3 (from data bus bit 19) controls the eorv output. If the bit is
	one, eorv outputs the NV/CSYNC signal from VIDC. If the bit is zero, eorv outputs inverted NV/CSYNC. Bit 2 of
	the register controls the eorh output. If the bit is zero, eorh is the NHSYNC output of VIDC. If the bit is one,
	eorh is inverted NHSYNC. Bits 1 and 0 control what is fed to the vidclk output as follows:

	     Bit1     Bit0     vidclk
	     0        0        24 Mhz clock
	     0        1        25 Mhz clock ;// external video crystal
	     1        0        36 Mhz clock
	     1        1        24 Mhz clock


	*/

	/*
	Golden Pyramids disassembly

	MOV     R0, #0x3200000
	ROM:03400948                 MOV     R1, #8
	ROM:0340094C                 STRB    R1, [R0,#0x14]  ; clear vsync
	ROM:03400950                 LDR     R2, =0xC350     ; 50000
	ROM:03400954
	ROM:03400954 loc_3400954                             ; CODE XREF: sub_3400944+18?j
	ROM:03400954                 NOP
	ROM:03400958                 SUBS    R2, R2, #1
	ROM:0340095C                 BNE     loc_3400954     ; does this 50000 times, presumably to wait for vsync
	ROM:03400960                 MOV     R0, #0x3200000
	ROM:03400964                 LDRB    R1, [R0,#0x10]  ; reads the irq status a
	ROM:03400968                 TST     R1, #8          ; test vsync
	*/


	archimedes_clear_irq_a(0x08);

	/*          bit 1              bit 0 */
	if((data &~(0x02)) && (data & (0x01))) // external video crystal is enabled. 25 mhz
	{
			m_mk5_VSYNC_timer->adjust(attotime::from_hz(50000)); // not sure but see above
	}
	if((data &~(0x02)) && (data &~(0x01))) // video clock is enabled. 24 mhz
	{
			m_mk5_VSYNC_timer->adjust(attotime::from_hz(50000)); // not sure
	}
	if((data & (0x02)) && (data &~(0x01))) // video clock is enabled. 36 mhz
	{
			m_mk5_VSYNC_timer->adjust(attotime::from_hz(50000)); // not sure
	}
	if((data &(0x02)) && (data &(0x01))) // video clock is enabled. 24 mhz
	{
			m_mk5_VSYNC_timer->adjust(attotime::from_hz(50000)); // not sure
	}
}

TIMER_CALLBACK_MEMBER(aristmk5_state::mk5_2KHz_callback)
{
	archimedes_request_irq_a(0x01);
	m_mk5_2KHz_timer->adjust(attotime::never);

}

READ32_MEMBER(aristmk5_state::Ns5x58)
{
	/*
	    1953.125 Hz for the operating system timer interrupt

	The pintr pin ( printer interrupt ) is connected to an interrupt latch in IOEB.
	A rising edge on pintr causes an interrupt to be latched in IOEB. The latch output
	is connected to the NIL[6] interrupt input on IOC and goes low when the rising edge is detected.
	The interrupt is cleared (NIL[6] is set high) by resetting the chip or by the NS5x58
	strobe.

	NIL[6] IOEB/1pintr - Interrupt Input ( OS Tick Interrput )

	Rising edge signal
	010101010101  .-------.   logic 0      .-------------.
	------------->|pint   |---1pintr------>|NIL[6]       |
	              | IOEB  |                |     IOC     |
	              `-------'                `-------------'
	*/


	// reset 2KHz timer
	m_mk5_2KHz_timer->adjust(attotime::from_hz(MASTER_CLOCK / 9 / 4096));
	archimedes_clear_irq_a(0x01);
	return 0xffffffff;
}

READ32_MEMBER(aristmk5_state::Ns5r50)
{
	return 0xf5; // checked inside the CPU check, unknown meaning
}

READ8_MEMBER(aristmk5_state::eeprom_r)
{
	uint8_t data = 0x00;
	if (m_eeprom[0]->do_read() && m_eeprom[1]->do_read())
		data |= 0x04;

	if (m_rtc->io_r())
		data |= 0x02;

	return data;
}

WRITE8_MEMBER(aristmk5_state::hopper_w)
{
	m_hopper->motor_w(BIT(data, 1));
	m_hopper_test = BIT(data, 2);
}

WRITE8_MEMBER(aristmk5_state::rtc_w)
{
	m_rtc->ce_w(BIT(data, 5));

	if (BIT(data, 6))
		m_rtc->io_w(BIT(data, 3));

	m_rtc->sclk_w(BIT(data, 4));
}

WRITE8_MEMBER(aristmk5_state::rtc_usa_w)
{
	rtc_w(space, offset, data, mem_mask);
	m_hopper_test = BIT(data, 2);
}

WRITE8_MEMBER(aristmk5_state::eeprom_w)
{
	m_coin_div = data & 1;

	m_eeprom[0]->cs_write(BIT(data, 5));
	m_eeprom[1]->cs_write(BIT(data, 6));
	m_eeprom[0]->di_write(BIT(data, 3));
	m_eeprom[1]->di_write(BIT(data, 3));
	m_eeprom[0]->clk_write(BIT(data, 4));
	m_eeprom[1]->clk_write(BIT(data, 4));
}

WRITE8_MEMBER(aristmk5_state::eeprom_usa_w)
{
	eeprom_w(space, offset, data, mem_mask);
	m_hopper->motor_w(BIT(data, 2));
}

READ8_MEMBER(aristmk5_state::ldor_r)
{
	if (m_extra_ports->read() & 0x01)
		m_ldor_shift_reg = 0;   // open the Logic door clears the shift register

	return (m_ldor_shift_reg & 0x80) | 0x60 | ((m_hopper_test && m_hopper->line_r()) ? 0x10 : 0x00);
}

WRITE8_MEMBER(aristmk5_state::ldor_clk_w)
{
	m_ldor_shift_reg = (m_ldor_shift_reg << 1) | BIT(data, 0);
}

WRITE8_MEMBER(aristmk5_state::sram_banksel_w)
{
	/*

	The Main Board provides 32 kbytes of Static Random Access Memory (SRAM) with
	battery back-up for the electronic meters.
	The SRAM contains machine metering information, recording money in/out and
	game history etc. It is critical that this data is preserved reliably, and various
	jurisdictions require multiple backups of the data.
	Three standard low power SRAMs are fitted to the board. The data is usually
	replicated three times, so that each chip contains identical data. Each memory is
	checked against the other to verify that the stored data is correct.
	Each chip is mapped to the same address, and the chip selected depends on the bank
	select register. Access is mutually exclusive, increasing security with only one chip
	visible in the CPU address space at a time. If the CPU crashes and overwrites
	memory only one of the three devices can be corrupted. On reset the bank select
	register selects bank 0, which does not exist. The SRAMs are at banks 1,2,3.
	Each of the SRAM chips may be powered from a separate battery, further reducing
	the possibility of losing data. For the US Gaming Machine, a single battery provides
	power for all three SRAMs. This battery also powers the Real Time Clock


	CHIP SELECT & SRAM BANKING

	write: 03010420 40  select bank 1
	write: 3220000 01   store 0x01 @ 3220000
	write: 03010420 80  select bank 2
	write: 3220000 02   store 0x02 @ 3220000
	write: 03010420 C0  ...
	write: 3220000 03   ...
	write: 03010420 00  ...
	write: 3220000 00   ...
	write: 03010420 40  select the first SRAM chip
	read:  3220000 01   read the value 0x1 back hopefully
	write: 03010420 80  ...
	read:  3220000 02   ...
	write: 03010420 C0  ...
	read:  3220000 03   ...
	write: 03010420 00  select bank 0


	     Bit 0 - Page 1
	     Bit 1 - Page 2
	     Bit 2 - Page 3
	     NC
	     NC
	     NC
	     Bit 6 - SRAM 1
	     Bit 7 - SRAM 2

	     Bit 1 and 2 on select Page 4.
	     Bit 6 and 7 on select SRAM 3.

	     4 pages of 32k for each sram chip.
	*/

	m_sram_bank = ((data & 0xc0) >> 3) | (data & 0x07);
}

WRITE8_MEMBER(aristmk5_state::buttons_lamps_w)
{
	for(int i = 0; i < 8; i++)
		m_lamps[(offset >> 2) * 8 + i] = BIT(data, i);
}

WRITE8_MEMBER(aristmk5_state::other_lamps_w)
{
	for(int i = 0; i < 8; i++)
		m_lamps[16 + i] = BIT(data, i);
}

WRITE8_MEMBER(aristmk5_state::bill_acceptor_lamps_w)
{
	for(int i = 0; i < 8; i++)
		m_lamps[24 + i] = BIT(data, i);
}

void aristmk5_state::aristmk5_map(address_map &map)
{
	map(0x02000000, 0x02ffffff).ram().share("physicalram"); /* physical RAM - 16 MB for now, should be 512k for the A310 */

	map(0x03000000, 0x0331ffff).rw(FUNC(aristmk5_state::archimedes_ioc_r), FUNC(aristmk5_state::archimedes_ioc_w));

	/* MK-5 overrides */
	map(0x03010420, 0x03010420).w(FUNC(aristmk5_state::sram_banksel_w)); // SRAM bank select write

	map(0x03010480, 0x0301049f).rw("uart_0a", FUNC(ins8250_uart_device::ins8250_r), FUNC(ins8250_uart_device::ins8250_w)).umask32(0x000000ff);
	map(0x03010500, 0x0301051f).rw("uart_0b", FUNC(ins8250_uart_device::ins8250_r), FUNC(ins8250_uart_device::ins8250_w)).umask32(0x000000ff);
	map(0x03010580, 0x03010583).portr("P3");
	map(0x03010600, 0x0301061f).rw("uart_1a", FUNC(ins8250_uart_device::ins8250_r), FUNC(ins8250_uart_device::ins8250_w)).umask32(0x000000ff);
	map(0x03010680, 0x0301069f).rw("uart_1b", FUNC(ins8250_uart_device::ins8250_r), FUNC(ins8250_uart_device::ins8250_w)).umask32(0x000000ff);

	map(0x03010700, 0x03010703).portr("P6");
	map(0x03010800, 0x03010800).r(FUNC(aristmk5_state::eeprom_r));
	map(0x03010810, 0x03010813).rw("watchdog", FUNC(watchdog_timer_device::reset32_r), FUNC(watchdog_timer_device::reset32_w)); //MK-5 specific, watchdog
	map(0x03220000, 0x0323ffff).rw(FUNC(aristmk5_state::sram_r), FUNC(aristmk5_state::sram_w)).umask32(0x000000ff);

	// bank5 slow
	map(0x03250048, 0x0325004b).w(FUNC(aristmk5_state::Ns5w48)); //IOEB control register
	map(0x03250050, 0x03250053).r(FUNC(aristmk5_state::Ns5r50)); //IOEB ID register
	map(0x03250058, 0x0325005b).r(FUNC(aristmk5_state::Ns5x58)); //IOEB interrupt Latch

	map(0x03320000, 0x0333ffff).rw(FUNC(aristmk5_state::sram_r), FUNC(aristmk5_state::sram_w)).umask32(0x000000ff);

	map(0x03400000, 0x035fffff).w(FUNC(aristmk5_state::archimedes_vidc_w));
	map(0x03600000, 0x037fffff).w(FUNC(aristmk5_state::archimedes_memc_w));
	map(0x03800000, 0x039fffff).w(FUNC(aristmk5_state::archimedes_memc_page_w));

	map(0x03400000, 0x03bfffff).rom().region("maincpu", 0);
}

/* U.S games have no dram emulator enabled */
void aristmk5_state::aristmk5_usa_map(address_map &map)
{
	aristmk5_map(map);

	map(0x00000000, 0x01ffffff).rw(FUNC(aristmk5_state::archimedes_memc_logical_r), FUNC(aristmk5_state::archimedes_memc_logical_w));

	map(0x03010440, 0x03010440).w(FUNC(aristmk5_state::rtc_usa_w));
	map(0x03010450, 0x03010450).w(FUNC(aristmk5_state::eeprom_usa_w));

	map(0x03012000, 0x03012003).portr("P1");
	map(0x03012010, 0x03012013).portr("P2");
	map(0x03012200, 0x03012203).portr("DSW1");
	map(0x03012210, 0x03012213).portr("DSW2");
	map(0x03010584, 0x03010587).portr("P4");

	map(0x03012020, 0x03012020).r(FUNC(aristmk5_state::ldor_r));
	map(0x03012070, 0x03012070).w(FUNC(aristmk5_state::ldor_clk_w));
	map(0x03012184, 0x03012187).portr("P5");

	map(0x03012000, 0x0301201f).w(FUNC(aristmk5_state::buttons_lamps_w)).umask32(0x000000ff);
	map(0x03012030, 0x0301203f).w(FUNC(aristmk5_state::other_lamps_w)).umask32(0x000000ff);
	map(0x03012380, 0x0301238f).w(FUNC(aristmk5_state::bill_acceptor_lamps_w)).umask32(0x000000ff);

	map(0x03012100, 0x0301211f).rw("uart_2a", FUNC(ins8250_uart_device::ins8250_r), FUNC(ins8250_uart_device::ins8250_w)).umask32(0x000000ff);
	map(0x03012140, 0x0301215f).rw("uart_2b", FUNC(ins8250_uart_device::ins8250_r), FUNC(ins8250_uart_device::ins8250_w)).umask32(0x000000ff);
	map(0x03012300, 0x0301231f).rw("uart_3a", FUNC(ins8250_uart_device::ins8250_r), FUNC(ins8250_uart_device::ins8250_w)).umask32(0x000000ff);
	map(0x03012340, 0x0301235f).rw("uart_3b", FUNC(ins8250_uart_device::ins8250_r), FUNC(ins8250_uart_device::ins8250_w)).umask32(0x000000ff);
}

/* with dram emulator enabled */
void aristmk5_state::aristmk5_drame_map(address_map &map)
{
	aristmk5_map(map);

	map(0x00000000, 0x01ffffff).rw(FUNC(aristmk5_state::aristmk5_drame_memc_logical_r), FUNC(aristmk5_state::archimedes_memc_logical_w));

	map(0x03010430, 0x03010430).w(FUNC(aristmk5_state::hopper_w));
	map(0x03010440, 0x03010440).w(FUNC(aristmk5_state::rtc_w));
	map(0x03010450, 0x03010450).w(FUNC(aristmk5_state::eeprom_w));

	map(0x03010400, 0x03010400).w(FUNC(aristmk5_state::spi_mux_w));
	map(0x03010470, 0x03010470).w(FUNC(aristmk5_state::spi_data_w));
	map(0x03010850, 0x03010850).rw(FUNC(aristmk5_state::spi_int_ack_r), FUNC(aristmk5_state::spi_int_ack_w));
	map(0x03010870, 0x03010870).r(FUNC(aristmk5_state::spi_data_r));

	map(0x03014000, 0x0301401f).rw("uart_2a", FUNC(ins8250_uart_device::ins8250_r), FUNC(ins8250_uart_device::ins8250_w)).umask32(0x000000ff);
	map(0x03014020, 0x0301403f).rw("uart_2b", FUNC(ins8250_uart_device::ins8250_r), FUNC(ins8250_uart_device::ins8250_w)).umask32(0x000000ff);
}


CUSTOM_INPUT_MEMBER(aristmk5_state::hopper_r)
{
	return (m_hopper_test && m_hopper->line_r()) ? 0 : 1;
}

CUSTOM_INPUT_MEMBER(aristmk5_state::coin_usa_r)
{
	//  ---x  Coin Acceptor
	//  --x-  Credit Sense
	//  -x--  Error Signal
	//  x---  Diverter Optic

	uint8_t data = 0x07;

	if (!m_coin_div)
		data |= 0x08;

	if (m_coin_start_cycles)
	{
		attotime diff = m_maincpu->cycles_to_attotime(m_maincpu->total_cycles() - m_coin_start_cycles);

		if (diff > attotime::from_msec(5) && diff < attotime::from_msec(10))
			data &= ~0x01;
		if (diff > attotime::from_msec(15) && diff < attotime::from_msec(20))
			data &= ~0x02;
		if (diff <= attotime::from_msec(3))
			data |= 0x08;

		if (diff > attotime::from_msec(30))
			m_coin_start_cycles = 0;
	}

	return data;
}

CUSTOM_INPUT_MEMBER(aristmk5_state::coin_r)
{
	uint8_t data = 0x01;

	if (m_coin_start_cycles)
	{
		attotime diff = m_maincpu->cycles_to_attotime(m_maincpu->total_cycles() - m_coin_start_cycles);

		if (diff > attotime::from_msec(10) && diff < attotime::from_msec(15))
			data &= ~0x01;
		if (diff > attotime::from_msec(0) && diff < attotime::from_msec(20))
			data |= 0x10;
		if (diff > attotime::from_msec(15) && diff < attotime::from_msec(30))
			data |= 0x08;
		if (diff > attotime::from_msec(25) && !m_coin_div)
			data |= 0x02;

		if (diff > attotime::from_msec(30))
			m_coin_start_cycles = 0;
	}

	return data;
}

INPUT_CHANGED_MEMBER(aristmk5_state::coin_start)
{
	if (newval && !m_coin_start_cycles)
		m_coin_start_cycles = m_maincpu->total_cycles();
}

static INPUT_PORTS_START( aristmk5_usa )
	/* This simulates the ROM swap */
	PORT_START("ROM_LOAD")
	PORT_CONFNAME( 0x07, 0x07, "System Mode" )
	PORT_CONFSETTING(    0x00, "USA Set Chip v4.04.09 Mode" )
	PORT_CONFSETTING(    0x01, "USA Set Chip v4.04.08 Mode" )
	PORT_CONFSETTING(    0x02, "USA Set Chip v4.04.05 Mode" )
	PORT_CONFSETTING(    0x03, "USA Set Chip v4.04.00 Mode" )
	PORT_CONFSETTING(    0x04, "USA Set Chip v4.03.07 Mode" )
	PORT_CONFSETTING(    0x05, "USA Set Chip v4.02.04 Mode" )
	PORT_CONFSETTING(    0x06, "USA Set Chip v1.0 Mode" )
	PORT_CONFSETTING(    0x07, "Game Mode" )

	PORT_START("DSW1")
	PORT_DIPNAME(0x0f, 0x0f, "Bank 1 - Denomination Values: Coin Value")
	PORT_DIPSETTING(0x0f, "$0.05") PORT_DIPLOCATION("DSW1:1,2,3,4")
	PORT_DIPSETTING(0x0e, "$0.10")
	PORT_DIPSETTING(0x0d, "$0.25")
	PORT_DIPSETTING(0x0c, "$0.50")
	PORT_DIPSETTING(0x0b, "$1.00")
	PORT_DIPSETTING(0x0a, "$5.00")
	PORT_DIPSETTING(0x09, "$10.00")
	PORT_DIPSETTING(0x08, "$50.00")
	PORT_DIPSETTING(0x07, "$100.00") // 0x00 through 0x07 are all $100.00
	PORT_DIPSETTING(0x06, "$100.00") // 0x00 through 0x06 retained for dip switch test
	PORT_DIPSETTING(0x05, "$100.00")
	PORT_DIPSETTING(0x04, "$100.00")
	PORT_DIPSETTING(0x03, "$100.00")
	PORT_DIPSETTING(0x02, "$100.00")
	PORT_DIPSETTING(0x01, "$100.00")
	PORT_DIPSETTING(0x00, "$100.00")
	PORT_DIPNAME(0xf0, 0xf0, "Bank 1 - Denomination Values: Credit Value")
	PORT_DIPSETTING(0xf0, "$0.01") PORT_DIPLOCATION("DSW1:5,6,7,8")
	PORT_DIPSETTING(0xe0, "$0.02")
	PORT_DIPSETTING(0xd0, "$0.05")
	PORT_DIPSETTING(0xc0, "$0.10")
	PORT_DIPSETTING(0xb0, "$0.25")
	PORT_DIPSETTING(0xa0, "$0.50")
	PORT_DIPSETTING(0x90, "$1.00")
	PORT_DIPSETTING(0x80, "$5.00")
	PORT_DIPSETTING(0x70, "$10.00")
	PORT_DIPSETTING(0x60, "$50.00")
	PORT_DIPSETTING(0x50, "$100.00") // 0x00 through 0x50 are all $100.00
	PORT_DIPSETTING(0x40, "$100.00") // All dip switches retained for dip switch test
	PORT_DIPSETTING(0x30, "$100.00")
	PORT_DIPSETTING(0x20, "$100.00")
	PORT_DIPSETTING(0x10, "$100.00")
	PORT_DIPSETTING(0x00, "$100.00")

	PORT_START("DSW2")
	PORT_DIPNAME(0x01, 0x00, "Bank 2: Gamble")
	PORT_DIPSETTING(0x01, "Gamble Not Allowed") PORT_DIPLOCATION("DSW2:1")
	PORT_DIPSETTING(0x00, "Gamble Allowed")
	PORT_DIPNAME(0x02, 0x02, "Bank 2: Max Bet Coin Reject")
	PORT_DIPSETTING(0x02, "Max Bet Coin Reject Not Enabled") PORT_DIPLOCATION("DSW2:2")
	PORT_DIPSETTING(0x00, "Max Bet Coin Reject Enabled")
	PORT_DIPNAME(0x04, 0x00, "Bank 2: Cancel Credit Type")
	PORT_DIPSETTING(0x04, "Residual Credit Backout Not Allowed") PORT_DIPLOCATION("DSW2:3")
	PORT_DIPSETTING(0x00, "Residual Credit Backout Allowed")
	PORT_DIPNAME(0x08, 0x00, "Bank 2: Bill Acceptor")
	PORT_DIPSETTING(0x08, "V2.2 - CRC Check") PORT_DIPLOCATION("DSW2:4")
	PORT_DIPSETTING(0x00, "VFM4 - No CRC Check")
	PORT_DIPNAME(0x30, 0x30, "Bank 2: BACC Denomination")
	PORT_DIPSETTING(0x30, "Dollar ($)") PORT_DIPLOCATION("DSW2:5,6")
	PORT_DIPSETTING(0x20, "Soles (S/.)")
	PORT_DIPSETTING(0x10, "UrPeso ($)")
	PORT_DIPSETTING(0x00, "Real (R/.)")
	PORT_DIPNAME(0x40, 0x40, "Bank 2: NOT USED")
	PORT_DIPSETTING(0x40, DEF_STR( Off )) PORT_DIPLOCATION("DSW2:7")
	PORT_DIPSETTING(0x00, DEF_STR( On ))
	PORT_DIPNAME(0x80, 0x80, "Bank 2: NOT USED")
	PORT_DIPSETTING(0x80, DEF_STR( Off )) PORT_DIPLOCATION("DSW2:8")
	PORT_DIPSETTING(0x00, DEF_STR( On ))

/*
    Button order in the Key Test menu is right to left; Gamble and Take Win are also reversed.
    On a typical 90 credit machine such as goldpyr, the button order is Service, Bet 1, Bet 2, Bet 3, Bet 5, Bet 10, Gamble, Take Win
    The button order has been designed to match the button panel rather than the test menu.
*/
	PORT_START("P1") // Bottom row
	PORT_BIT(0x00000080, IP_ACTIVE_HIGH, IPT_BUTTON7)  PORT_CODE(KEYCODE_A) PORT_NAME("Service")                // PLAYER KEY 8
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("Bet 1 Credit / Red")     // PLAYER KEY 7
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Bet 2 Credits")          // PLAYER KEY 6
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Bet 3 Credits")          // PLAYER KEY 5
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Bet 5 Credits")          // PLAYER KEY 4
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Bet 10 Credits / Black") // PLAYER KEY 3
	PORT_BIT(0x00000001, IP_ACTIVE_HIGH, IPT_BUTTON13) PORT_CODE(KEYCODE_J) PORT_NAME("Gamble")                 // PLAYER KEY 1
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win")               // PLAYER KEY 2

	PORT_START("P2") // Top row
	PORT_BIT(0x00000001, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_CODE(KEYCODE_Q) PORT_NAME("Cashout")                 // PLAYER KEY 9
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_CODE(KEYCODE_W) PORT_NAME("Play 1 Line")             // PLAYER KEY 10
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_CODE(KEYCODE_E) PORT_NAME("Play 3 Lines")            // PLAYER KEY 11
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON4) PORT_CODE(KEYCODE_R) PORT_NAME("Play 5 Lines")            // PLAYER KEY 12
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON5) PORT_CODE(KEYCODE_T) PORT_NAME("Play 7 Lines")            // PLAYER KEY 13
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON6) PORT_CODE(KEYCODE_Y) PORT_NAME("Play 9 Lines")            // PLAYER KEY 14
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00000080, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("P3")
	PORT_BIT(0x00000001, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_SERVICE) PORT_NAME("Audit Key")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_GAMBLE_KEYIN) PORT_CODE(KEYCODE_V) PORT_NAME("Jackpot/Reset Key")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_GAMBLE_DOOR)  PORT_CODE(KEYCODE_B) PORT_TOGGLE PORT_NAME("Bill acceptor door")
	PORT_BIT(0x00000040, IP_ACTIVE_LOW,  IPT_GAMBLE_DOOR)  PORT_CODE(KEYCODE_M) PORT_TOGGLE PORT_NAME("Main door")
	PORT_BIT(0x00000080, IP_ACTIVE_HIGH, IPT_GAMBLE_DOOR)  PORT_CODE(KEYCODE_C) PORT_TOGGLE PORT_NAME("Cashbox door")

	PORT_START("P4")
	PORT_BIT(0x00000078, IP_ACTIVE_HIGH, IPT_CUSTOM) PORT_CUSTOM_MEMBER(DEVICE_SELF, aristmk5_state, coin_usa_r, nullptr)

	PORT_START("P5")
	PORT_BIT(0x00000008, IP_ACTIVE_LOW,  IPT_OTHER)   // Meters

	PORT_START("P6")
	PORT_BIT(0x00000002, IP_ACTIVE_LOW, IPT_OTHER)    // Battery

	PORT_START("EXTRA")
	PORT_BIT(0x00000001, IP_ACTIVE_HIGH, IPT_OTHER)   PORT_TOGGLE PORT_CODE(KEYCODE_L)   PORT_NAME("Logic door")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_COIN1)   PORT_CHANGED_MEMBER(DEVICE_SELF, aristmk5_state, coin_start, nullptr)
INPUT_PORTS_END

static INPUT_PORTS_START( aristmk5 )

	PORT_START("P1")
	PORT_BIT(0x00000100, IP_ACTIVE_HIGH, IPT_BUTTON1)  PORT_CODE(KEYCODE_Q) PORT_NAME("Collect")
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_BUTTON2)  PORT_CODE(KEYCODE_W) PORT_NAME("Bet 1 Credit / Heart")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Bet 2 Credits / Diamond")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 3 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 5 Credits / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 10 Credits / Spade")
	PORT_BIT(0x00000080, IP_ACTIVE_HIGH, IPT_BUTTON7)  PORT_CODE(KEYCODE_A) PORT_NAME("Reserve")
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("Play 1 Line / Red")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Play 5 Lines")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Play 10 Lines")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 15 Lines")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Play 20 Lines / Black")
	PORT_BIT(0x00000001, IP_ACTIVE_HIGH, IPT_BUTTON13) PORT_CODE(KEYCODE_J) PORT_NAME("Gamble")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win / Start Feature")
	PORT_BIT(0x00004000, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00008000, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00ff0000, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("P2")
	PORT_BIT(0x00000001, IP_ACTIVE_HIGH, IPT_GAMBLE_KEYIN) PORT_CODE(KEYCODE_V) PORT_NAME("Jackpot/Reset Key")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_SERVICE) PORT_NAME("Audit Key")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_OTHER)   // Hopper full
	PORT_BIT(0x00000080, IP_ACTIVE_HIGH, IPT_OTHER)   // Hopper empty
	PORT_BIT(0x00000100, IP_ACTIVE_LOW,  IPT_GAMBLE_DOOR)  PORT_CODE(KEYCODE_M) PORT_TOGGLE PORT_NAME("Main door optical sensor")
	PORT_BIT(0x0000fe00, IP_ACTIVE_HIGH, IPT_UNUSED)  // Unused optical security sensors
	PORT_BIT(0x00010000, IP_ACTIVE_HIGH, IPT_GAMBLE_DOOR)  PORT_CODE(KEYCODE_L) PORT_TOGGLE PORT_NAME("Logic door")
	PORT_BIT(0x00020000, IP_ACTIVE_HIGH, IPT_GAMBLE_DOOR)  PORT_CODE(KEYCODE_Z) PORT_TOGGLE PORT_NAME("Topbox door")
	PORT_BIT(0x00040000, IP_ACTIVE_HIGH, IPT_GAMBLE_DOOR)  PORT_CODE(KEYCODE_X) PORT_TOGGLE PORT_NAME("Meter cage")
	PORT_BIT(0x00080000, IP_ACTIVE_HIGH, IPT_GAMBLE_DOOR)  PORT_CODE(KEYCODE_C) PORT_TOGGLE PORT_NAME("Cashbox door")
	PORT_BIT(0x00100000, IP_ACTIVE_LOW,  IPT_GAMBLE_DOOR)  PORT_CODE(KEYCODE_M) PORT_TOGGLE PORT_NAME("Main door")
	PORT_BIT(0x00200000, IP_ACTIVE_HIGH, IPT_GAMBLE_DOOR)  PORT_CODE(KEYCODE_B) PORT_TOGGLE PORT_NAME("Bill acceptor door")
	PORT_BIT(0x00c00000, IP_ACTIVE_HIGH, IPT_UNUSED)  // Unused mechanical security switch

PORT_START("P3")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_KEYPAD)  PORT_CUSTOM_MEMBER(DEVICE_SELF, aristmk5_state, hopper_r, nullptr)
	PORT_BIT(0x000000f8, IP_ACTIVE_HIGH, IPT_CUSTOM) PORT_CUSTOM_MEMBER(DEVICE_SELF, aristmk5_state, coin_r, nullptr)

	PORT_START("P6")
	PORT_BIT(0x00000002, IP_ACTIVE_LOW, IPT_OTHER)    // Battery

	PORT_START("EXTRA")
	PORT_BIT(0x00000001, IP_ACTIVE_HIGH, IPT_COIN1)   PORT_CHANGED_MEMBER(DEVICE_SELF, aristmk5_state, coin_start, nullptr)
INPUT_PORTS_END

/********** Game-specific button labels **********/

static INPUT_PORTS_START(aristmk5_9)
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Play 3 Lines")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Play 5 Lines")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 7 Lines")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Play 9 Lines / Black")
INPUT_PORTS_END

static INPUT_PORTS_START(trstrove)
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 5 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 10 Credits / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 25 Credits / Spade")
INPUT_PORTS_END

static INPUT_PORTS_START(qnile)
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Bet 5 Credits / Diamond")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 10 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 20 Credits / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 25 Credits / Spade")
INPUT_PORTS_END

static INPUT_PORTS_START(cashcham)
	PORT_INCLUDE(qnile)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win")
INPUT_PORTS_END

static INPUT_PORTS_START(cashchama)
	PORT_INCLUDE(cashcham)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Bet 2 Credits / Diamond")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 3 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 4 Credits / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 5 Credits / Spade")
INPUT_PORTS_END

static INPUT_PORTS_START(cashchamnz)
	PORT_INCLUDE(cashchama)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 15 Lines / Autoplay")
INPUT_PORTS_END

static INPUT_PORTS_START(cashcatnz)
	PORT_INCLUDE(aristmk5_9)

	PORT_MODIFY("P1")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 4 Credits / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 5 Credits / Spade")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 7 Lines / Autoplay")
INPUT_PORTS_END

static INPUT_PORTS_START(incasunnz)
	PORT_INCLUDE(cashchamnz)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win / Start Feature")
INPUT_PORTS_END

static INPUT_PORTS_START(chariotc)
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("Play 1 Line / Chariot 1 / Red")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Play 5 Lines / Chariot 2")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Play 10 Lines / Chariot 3")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 15 Lines / Chariot 4")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Play 20 Lines / Chariot 5 / Black")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win")
INPUT_PORTS_END

static INPUT_PORTS_START(chariotcv)
	PORT_INCLUDE(chariotc)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Play 3 Lines / Chariot 2")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Play 5 Lines / Chariot 3")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 7 Lines / Chariot 4")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Play 9 Lines / Chariot 5 / Black")
INPUT_PORTS_END

static INPUT_PORTS_START(geisha)
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 12 Credits / Spade")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 15 Lines / Autoplay")
INPUT_PORTS_END

static INPUT_PORTS_START(one4all)
	PORT_INCLUDE(geisha)

	PORT_MODIFY("P1")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 6 Credits / Spade")
INPUT_PORTS_END

static INPUT_PORTS_START(montree)
	PORT_INCLUDE(geisha)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win")
INPUT_PORTS_END

static INPUT_PORTS_START(goldenra) // marmagic uses the same button panel as goldenra for 1000 credit play, however it has a number of other bet and line options
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 5 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 25 Credits / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 50 Credits / Spade")
INPUT_PORTS_END

static INPUT_PORTS_START(dynajack) // button panel reflects 1000 credit option only
	PORT_INCLUDE(goldenra)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win / Free Spin")
INPUT_PORTS_END

static INPUT_PORTS_START(goldpyrb)
	PORT_INCLUDE(aristmk5_9)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000080, IP_ACTIVE_HIGH, IPT_BUTTON7)  PORT_CODE(KEYCODE_A) PORT_NAME("Service")
INPUT_PORTS_END

static INPUT_PORTS_START(penpir2)
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000080, IP_ACTIVE_HIGH, IPT_BUTTON7)  PORT_CODE(KEYCODE_A) PORT_NAME("Service")
INPUT_PORTS_END

static INPUT_PORTS_START(incasun)
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 25 Credits / Spade")
INPUT_PORTS_END

static INPUT_PORTS_START(coralrc2)
	PORT_INCLUDE(incasun)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win")
INPUT_PORTS_END

static INPUT_PORTS_START(genmagi)
	PORT_INCLUDE(incasun)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_BUTTON2)  PORT_CODE(KEYCODE_W) PORT_NAME("Bet 1 Credit")  // No red/black/suits on the buttons
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Bet 2 Credits") // Gamble feature is touchscreen-based
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 5 Credits")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 25 Credits")
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("Play 1 Line")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Play 20 Lines")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win / Start Free Games")
INPUT_PORTS_END

static INPUT_PORTS_START(adonisce)
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("Play 1 Line / Red / Train 1")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Play 5 Lines / Train 2")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Play 10 Lines / Train 3")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 15 Lines / Train 4")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Play 20 Lines / Black / Train 5")
INPUT_PORTS_END

static INPUT_PORTS_START(kgalah)
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win")
INPUT_PORTS_END

static INPUT_PORTS_START(petshop) // different input order, weird
	PORT_INCLUDE(kgalah)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON2)  PORT_CODE(KEYCODE_W) PORT_NAME("Bet 1 Credit / Heart")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Bet 2 Credits / Diamond")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 3 Credits / Club")
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 5 Credits")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("Play 1 Line / Red")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Play 5 Lines")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Play 10 Lines")
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 15 Lines")
INPUT_PORTS_END

static INPUT_PORTS_START(rushrst)
	PORT_INCLUDE(cashchama)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 15 Lines / Run")
INPUT_PORTS_END

static INPUT_PORTS_START(mystgard)
	PORT_INCLUDE(kgalah)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_BUTTON2)  PORT_CODE(KEYCODE_W) PORT_NAME("Bet 1 Credit")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Bet 2 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 4 Credits")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 5 Credits")
INPUT_PORTS_END

static INPUT_PORTS_START(mountmon)
	PORT_INCLUDE(mystgard)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Bet 5 Credits")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 10 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 25 Credits")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 50 Credits")
INPUT_PORTS_END

static INPUT_PORTS_START(qnilec)
	PORT_INCLUDE(aristmk5_9)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 5 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 10 Credits / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 20 Credits / Spade")
INPUT_PORTS_END

static INPUT_PORTS_START(locoloota)
	PORT_INCLUDE(qnilec)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win")
INPUT_PORTS_END

static INPUT_PORTS_START(snowcat)
	PORT_INCLUDE(aristmk5_9)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win")
INPUT_PORTS_END

static INPUT_PORTS_START(chickna5)
	PORT_INCLUDE(snowcat)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 7 Lines / Run")
INPUT_PORTS_END

static INPUT_PORTS_START(chickna5v)
	PORT_INCLUDE(chickna5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Bet 5 Credits / Diamond")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 10 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 25 Credits / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 50 Credits / Spade")
INPUT_PORTS_END

static INPUT_PORTS_START(orchidms)
	PORT_INCLUDE(chickna5v)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Play 2 Lines")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Play 4 Lines")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 8 Lines")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Play 10 Lines / Black")
INPUT_PORTS_END

static INPUT_PORTS_START(orchidmsnz)
	PORT_INCLUDE(orchidms)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Bet 2 Credits / Diamond")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 5 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 15 Credits / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 25 Credits / Spade")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 8 Lines / Autoplay")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win / Start Feature")
INPUT_PORTS_END

static INPUT_PORTS_START(pantmaga)
	PORT_INCLUDE(snowcat)

	PORT_MODIFY("P1")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Spade")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Play 2 Lines")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Play 3 Lines")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 4 Lines")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Play 5 Lines / Black")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_UNUSED)
INPUT_PORTS_END

static INPUT_PORTS_START(sbuk3)
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Bet 1 Credit")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 2 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 3 Credits")
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("x2")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Play 1 Line / x3")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Play 2 Lines / x5")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 3 Lines / x10")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("x100")
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_UNUSED)
INPUT_PORTS_END

static INPUT_PORTS_START(retrsam)
	PORT_INCLUDE(sbuk3)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win")
INPUT_PORTS_END

static INPUT_PORTS_START(swhr2)
	PORT_INCLUDE(aristmk5_9)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_BUTTON2)  PORT_CODE(KEYCODE_W) PORT_NAME("Bet 1 Credit")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Bet 2 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 5 Credits")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 10 Credits")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win")
INPUT_PORTS_END

static INPUT_PORTS_START(retrsamb)
	PORT_INCLUDE(swhr2)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("Play 1 Line / x2")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Play 3 Lines / x3")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Play 5 Lines / x5")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 7 Lines / x10")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Play 9 Lines / x100")
INPUT_PORTS_END

static INPUT_PORTS_START(toutangonl)
	PORT_INCLUDE(swhr2)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000080, IP_ACTIVE_HIGH, IPT_BUTTON7)  PORT_CODE(KEYCODE_A) PORT_NAME("Service")
INPUT_PORTS_END

static INPUT_PORTS_START(qnilenl)
	PORT_INCLUDE(toutangonl)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win / Start Feature")
INPUT_PORTS_END

static INPUT_PORTS_START(dimtouch)
	PORT_INCLUDE(swhr2)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("Play 1 Line")  // Gamble feature is touchscreen-based
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 7 Lines / Autoplay")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Play 9 Lines") // No red/black/suits on the buttons
INPUT_PORTS_END

static INPUT_PORTS_START(qtbird)
	PORT_INCLUDE(swhr2)

	PORT_MODIFY("P1")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 4 Credits")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 5 Credits")
INPUT_PORTS_END

static INPUT_PORTS_START(jungjuic)
	PORT_INCLUDE(qtbird)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000080, IP_ACTIVE_HIGH, IPT_BUTTON7)  PORT_CODE(KEYCODE_A) PORT_NAME("Service")
INPUT_PORTS_END

static INPUT_PORTS_START(wcougar)
	PORT_INCLUDE(swhr2)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 5 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 10 Credits")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 20 Credits")
INPUT_PORTS_END

static INPUT_PORTS_START(dreamwv)
	PORT_INCLUDE(wcougar)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("Play 1 Line")  // Gamble feature is touchscreen-based
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 7 Lines / Autoplay")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Play 9 Lines") // No red/black/suits on the buttons
INPUT_PORTS_END

static INPUT_PORTS_START(kgbirda5)
	PORT_INCLUDE(swhr2)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Play 2 Lines")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Play 3 Lines")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 4 Lines")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Play 5 Lines / Black")
INPUT_PORTS_END

static INPUT_PORTS_START(checkma5) // 3 line button panel only
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_BUTTON2)  PORT_CODE(KEYCODE_W) PORT_NAME("Heart")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Bet 1 Credit / Diamond")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 2 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 3 Credits / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Spade")
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("Red")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Play 1 Line")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Play 2 Lines")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 3 Lines")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Black")
INPUT_PORTS_END

static INPUT_PORTS_START(wildbill)
	PORT_INCLUDE(checkma5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win")
INPUT_PORTS_END

static INPUT_PORTS_START(przfight)
	PORT_INCLUDE(wildbill)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Bet 1 Credit")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 3 Credits")
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_UNUSED)
INPUT_PORTS_END

static INPUT_PORTS_START(wamazon)
	PORT_INCLUDE(wildbill)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Play Feature Game / Black")
INPUT_PORTS_END

static INPUT_PORTS_START(wamazona)
	PORT_INCLUDE(wildbill)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Diamond")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Club")
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("Bet 1 Credit / Red")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Bet 2 Credits")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Bet 3 Credits")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Bet 5 Credits")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Bet 10 Credits / Black")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_UNUSED)
INPUT_PORTS_END

static INPUT_PORTS_START(sbuk2)
	PORT_INCLUDE(kgalah)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("Bet 1 Credit / Red")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Bet 2 Credits")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Bet 3 Credits")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Bet 4 Credits")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Bet 5 Credits / Black")
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_UNUSED)
INPUT_PORTS_END

static INPUT_PORTS_START(unicorndnz)
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 15 Lines / Autoplay")
INPUT_PORTS_END

static INPUT_PORTS_START(wtiger)
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000100, IP_ACTIVE_HIGH, IPT_BUTTON1)  PORT_CODE(KEYCODE_Q) PORT_NAME("Take Win / Collect")
	PORT_BIT(0x00000080, IP_ACTIVE_HIGH, IPT_BUTTON7)  PORT_CODE(KEYCODE_A) PORT_NAME("Gamble / Reserve")
	PORT_BIT(0x00000001, IP_ACTIVE_HIGH, IPT_BUTTON13) PORT_CODE(KEYCODE_J) PORT_NAME("No Classic Buy Feature")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Classic Buy Feature / Start Feature")
INPUT_PORTS_END

static INPUT_PORTS_START(indrema5)
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 5 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 10 Credits / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 20 Credits / Spade")
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("Play 3 Ways / Red")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Play 9 Ways")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Play 27 Ways")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Play 81 Ways")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Play 243 Ways / Black")
INPUT_PORTS_END

static INPUT_PORTS_START(wizways)
	PORT_INCLUDE(indrema5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON14) PORT_CODE(KEYCODE_K) PORT_NAME("Take Win")
INPUT_PORTS_END

static INPUT_PORTS_START(reelrock)
	PORT_INCLUDE(wizways)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 3 Credits")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 5 Credits / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Bet 8 Credits / Spade")
INPUT_PORTS_END

static INPUT_PORTS_START(wikwin)
	PORT_INCLUDE(reelrock)

	PORT_MODIFY("P1")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 4 Credits / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Max Bet / Spade")
INPUT_PORTS_END

static INPUT_PORTS_START(baddog)
	PORT_INCLUDE(aristmk5)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_BUTTON2)  PORT_CODE(KEYCODE_W) PORT_NAME("Bet 1 / Hold 1 / Heart")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Bet 2/5/50 / Hold 2 / Diamond")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Bet 5/25/100 / Hold 3")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Bet 10/50/200 / Hold 4 / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("High 10/50/200 / Hold 5 / Spade")
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_BUTTON8)  PORT_CODE(KEYCODE_S) PORT_NAME("Red")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON9)  PORT_CODE(KEYCODE_D) PORT_NAME("Gamble / Half Gamble")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_CODE(KEYCODE_F) PORT_NAME("Take Win")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Black")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Draw")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x00000001, IP_ACTIVE_HIGH, IPT_UNUSED)
INPUT_PORTS_END

static INPUT_PORTS_START(multidrw)
	PORT_INCLUDE(baddog)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_BUTTON2)  PORT_CODE(KEYCODE_W) PORT_NAME("Hold 1 / Heart")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Hold 2 / Diamond")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("1 Draw / Hold 3")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("2 Draws / Hold 4 / Club")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("3 Draws / Hold 5 / Spade")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Bet / Next Card")
	PORT_BIT(0x00000001, IP_ACTIVE_HIGH, IPT_BUTTON13) PORT_CODE(KEYCODE_J) PORT_NAME("Draw")
INPUT_PORTS_END

static INPUT_PORTS_START(chickna5u)
	PORT_INCLUDE(aristmk5_usa)

	PORT_MODIFY("P2")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Play 7 Lines / Run")
INPUT_PORTS_END

static INPUT_PORTS_START(dolphntru)
	PORT_INCLUDE(aristmk5_usa)

	PORT_MODIFY("P2")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Play 7 Lines / Start Feature")
INPUT_PORTS_END

static INPUT_PORTS_START(bootsctnua)
	PORT_INCLUDE(aristmk5_usa)

	PORT_MODIFY("P2")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Play 5 Lines")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Play 10 Lines")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Play 15 Lines")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Play 20 Lines")
INPUT_PORTS_END

static INPUT_PORTS_START(pengpuck)
	PORT_INCLUDE(aristmk5_usa)

	PORT_MODIFY("P2") // experimental use of PORT_CONDITION to select control panels
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Play 5 Lines")  PORT_CONDITION("CPANEL",0x01,NOTEQUALS,0x01)
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Play 10 Lines") PORT_CONDITION("CPANEL",0x01,NOTEQUALS,0x01)
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Play 15 Lines") PORT_CONDITION("CPANEL",0x01,NOTEQUALS,0x01)
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Play 20 Lines") PORT_CONDITION("CPANEL",0x01,NOTEQUALS,0x01)

	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Alt Button 1") PORT_CONDITION("CPANEL",0x01,EQUALS,0x01)
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Alt Button 2") PORT_CONDITION("CPANEL",0x01,EQUALS,0x01)
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Alt Button 3") PORT_CONDITION("CPANEL",0x01,EQUALS,0x01)
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Alt Button 4") PORT_CONDITION("CPANEL",0x01,EQUALS,0x01)

	PORT_START("CPANEL") // set this to match the game mode selected in Sevice Mode in addition to the layout
	PORT_CONFNAME( 0x00000001, 0x00000000, "Control Panel Type" )
	PORT_CONFSETTING(          0x00000000, "Normal" )
	PORT_CONFSETTING(          0x00000001, "Alt" )
INPUT_PORTS_END

static INPUT_PORTS_START(dolphntrce)
	PORT_INCLUDE(bootsctnua)

	PORT_MODIFY("P2")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Play 15 Lines / Start Feature")
INPUT_PORTS_END

static INPUT_PORTS_START(swhr2u)
	PORT_INCLUDE(aristmk5_usa)

	PORT_MODIFY("P1")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON11) PORT_CODE(KEYCODE_G) PORT_NAME("Bet 4 Credits")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON12) PORT_CODE(KEYCODE_H) PORT_NAME("Bet 5 Credits / Black")

	PORT_MODIFY("P2")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Play 2 Lines")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Play 3 Lines")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Play 4 Lines")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Play 5 Lines")
INPUT_PORTS_END

static INPUT_PORTS_START(wnpost)
	PORT_INCLUDE(aristmk5_usa)

	PORT_MODIFY("P2")
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_BUTTON2)  PORT_CODE(KEYCODE_W) PORT_NAME("Play 1 Line / Horse 1")
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_CODE(KEYCODE_E) PORT_NAME("Play 2 Lines / Horse 2")
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_CODE(KEYCODE_R) PORT_NAME("Play 3 Lines / Horse 3")
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_BUTTON5)  PORT_CODE(KEYCODE_T) PORT_NAME("Play 4 Lines / Horse 4")
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_BUTTON6)  PORT_CODE(KEYCODE_Y) PORT_NAME("Play 5 Lines / Horse 5")
INPUT_PORTS_END

void aristmk5_state::init_aristmk5()
{
	archimedes_driver_init();

	int do_debug = 0;
	if (do_debug)
	{
		// DEBUG code for showing the range of the ROMs that get checksummed (for adding to rom loading comments)
		// unfortunately the checksum only covers the code part of the ROM, leaving the data without any kind of
		// verification.  Given that the existing bad-dumps would be non-obvious if it the checksums weren't incorrect
		// this is potentially worrying.  The actual checksum scheme is weak too, a simple 32-bit add.

		uint32_t *ROM = (uint32_t*)memregion("game_prg")->base();
		int size = memregion("game_prg")->bytes();
		int found = 0;

		for (int i = 0; i < (size / 4) - 4; i++)
		{
			if (((ROM[i + 0] & 0xfffff000) == 0xe59f1000) &&
				(ROM[i + 1] == 0xe3a03000) &&
				((ROM[i + 2] & 0xfffff000) == 0xe59f4000) &&
				(ROM[i + 3] == 0xe0444001))
			{

				printf("Checksum code found at 0x%06x\n", i * 4);
				found = 1;

				int baseoff = ROM[i + 0] & 0x00000fff;
				int baseoff2 = ROM[i + 2] & 0x00000fff;
				int baseoff3 = ROM[i + 5] & 0x00000fff;
				int baseoff4 = ROM[i + 8] & 0x00000fff;

				//printf("values offset %08x %08x %08x %08x\n", baseoff, baseoff2, baseoff3, baseoff4);

				int actual = ROM[i + (baseoff / 4) + 2]; // base
				int actual2 = ROM[i + 2 + (baseoff2 / 4) + 2]; // end
				int actual3 = ROM[i + 5 + (baseoff3 / 4) + 2]; // skip end (where checksum is)
				int actual4 = ROM[i + 8 + (baseoff4 / 4) + 2]; // skip start (^)

				//printf("values %08x %08x %08x %08x\n", actual, actual2, actual3, actual4);

				actual2 = actual2 - actual;
				actual3 = actual3 - actual;
				actual4 = actual4 - actual;
				actual = 0;

				if ((actual4 - actual3) != 4)
				{
					printf("UNUSUAL SKIP RANGE %06x - %06x\n", actual3, actual4 + 3);
				}
				else
				{
					int expectedchecksum = ROM[actual3 / 4];
					printf("0x%06x-0x%06x is the Checksummed Range (excluding 0x%06x-0x%06x where Checksum is stored)\n    Expected Checksum   0x%08x\n", actual, actual2 - 1, actual3, actual4 + 3, expectedchecksum);

					// the checksum is a simple 32-bit sum with the dword containing the checksum skipped (and the dword after it for no obvious reason!)
					uint32_t calculatedchecksum = 0;
					for (int i = actual / 4; i < actual2 / 4; i++)
					{
						if ((i < (actual3 / 4)) ||
							(i > (actual4 / 4)))
						{
							calculatedchecksum += ROM[i];

							//  printf("Using address %08x, value %08x, Calculated Checksum %08x\n", i*4,  ROM[i], calculatedchecksum);
						}
						else
						{
							//  printf("SKIPPING address %08x, value %08x, Calculated Checksum %08x\n", i*4,  ROM[i], calculatedchecksum);
						}
					}

					printf("    Calculated Checksum 0x%08x ", calculatedchecksum);
					if (calculatedchecksum == expectedchecksum)
					{
						printf(" (OK)\n");
					}
					else
					{
						printf(" (BAD)\n");
					}

					size = size / 4;

					// almost always just the end of the roms
					int realend = 0;
					for (int i = size - 1; i >= actual2 / 4; i--)
					{
						if ((ROM[i] != 0xffffffff) && (ROM[i] != 0x00000000))
						{
							//printf("real data end at %08x\n", i * 4);
							realend = i;
							i = actual2 / 4 - 1;
						}
					}


					int realend2 = 0;
					for (int i = realend - 4; i >= actual2 / 4; i--)
					{
						if ((ROM[i] != 0xffffffff) && (ROM[i] != 0x00000000))
						{
							//printf("real data end at %08x (value is %08x)\n", i * 4, ROM[i]);
							realend2 = (i * 4) + 3;
							i = actual2 / 4 - 1;
						}
					}

					realend = (realend * 4) + 3;

					if ((realend & 0xffff) == 0xffff)
					{
						printf("0x%06x-0x%06x is the non-Checksummed range still containing data but NOT covered by Checksum\n", actual2, realend2);
						printf("0x%06x-0x%06x is the non-Checksummed range if the additional vectors? at the end are included\n", actual2, realend);
					}
					else
					{
						printf("0x%06x-0x%06x is the non-Checksummed range (unusual endpoint)\n", actual2, realend);
					}
				}
			}
		}

		if (found == 0)
		{
			printf("checksum code not found\n");
		}
	}
}


void aristmk5_state::machine_start()
{
	m_nvram->set_base(m_sram->base(), m_sram->bytes());

	archimedes_init();

	m_mk5_2KHz_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(aristmk5_state::mk5_2KHz_callback),this));
	m_mk5_VSYNC_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(aristmk5_state::mk5_VSYNC_callback),this));
	m_spi_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(aristmk5_state::spi_timer), this));

	m_lamps.resolve();
}

void aristmk5_state::machine_reset()
{
	archimedes_reset();
	m_mk5_2KHz_timer->adjust(attotime::from_hz(MASTER_CLOCK / 9 / 4096)); // 8MHz / 4096
	m_mk5_VSYNC_timer->adjust(attotime::from_hz(50000)); // default bit 1 & bit 2 == 0

	m_ioc_regs[IRQ_STATUS_B] |= 0x40; //hack, set keyboard irq empty to be ON

	{
		/* for US sets, load the roms according to what the operator wants */
		uint8_t *ROM = memregion("maincpu")->base();
		uint8_t *PRG;

		if (ioport("ROM_LOAD") != nullptr)
		{
			static const char *const rom_region[] = { "set_4.04.09", "set_4.04.08", "set_4.04.05", "set_4.04.00", "set_4.03.07", "set_4.02.04", "set_1.0", "game_prg" };

			uint8_t op_mode = ioport("ROM_LOAD")->read();

			PRG = memregion(rom_region[op_mode & 7])->base();
		}
		else // non-US sets don't have set chips
			PRG = memregion("game_prg")->base();

		for(int i = 0; i < 0x400000; i++)
			ROM[i] = PRG[i];
	}

	m_ldor_shift_reg = 0x55;
	m_coin_start_cycles = 0;
	m_sram_bank = 0;
	m_hopper_test = 1;
	m_coin_div = 0;
	m_spi_mux = 0;
	m_spi_latch = 0;
	m_spi_bits = 0;
	memset(m_spi_data, 0, sizeof(m_spi_data));
}

void aristmk5_state::aristmk5(machine_config &config)
{
	ARM(config, m_maincpu, MASTER_CLOCK/6); // 12000000
	m_maincpu->set_addrmap(AS_PROGRAM, &aristmk5_state::aristmk5_drame_map);

	WATCHDOG_TIMER(config, "watchdog").set_time(attotime::from_seconds(2));  /* 1.6 - 2 seconds */

	/* TODO: this isn't supposed to access a keyboard ... */
	AAKART(config, m_kart, 12000000/128); // TODO: frequency

	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_refresh_hz(60);
	m_screen->set_vblank_time(ATTOSECONDS_IN_USEC(0));
	m_screen->set_size(640, 400);
	m_screen->set_visarea(0, 640-1, 0, 400-1);
	m_screen->set_screen_update(FUNC(archimedes_state::screen_update));

	PALETTE(config, m_palette).set_entries(0x200);

	EEPROM_93C56_16BIT(config, m_eeprom[0]);
	EEPROM_93C56_16BIT(config, m_eeprom[1]);

	NVRAM(config, m_nvram, nvram_device::DEFAULT_NONE);

	// TL16C452FN U71
	ns16450_device &uart0a(NS16450(config, "uart_0a", MASTER_CLOCK / 9));
	uart0a.out_int_callback().set("uart_irq", FUNC(input_merger_device::in_w<0>));
	ns16450_device &uart0b(NS16450(config, "uart_0b", MASTER_CLOCK / 9));
	uart0b.out_int_callback().set("uart_irq", FUNC(input_merger_device::in_w<1>));

	// TL16C452FN U72
	ns16450_device &uart1a(NS16450(config, "uart_1a", MASTER_CLOCK / 9));
	uart1a.out_int_callback().set("uart_irq", FUNC(input_merger_device::in_w<2>));
	ns16450_device &uart1b(NS16450(config, "uart_1b", MASTER_CLOCK / 9));
	uart1b.out_int_callback().set("uart_irq", FUNC(input_merger_device::in_w<3>));

	// COMM port 4 - 5
	NS16450(config, "uart_2a", MASTER_CLOCK / 9);
//  MCFG_INS8250_OUT_INT_CB(WRITELINE(*this, FUNC(input_merger_device::in_w<4>));
	NS16450(config, "uart_2b", MASTER_CLOCK / 9);
//  MCFG_INS8250_OUT_INT_CB(WRITELINE(*this, FUNC(input_merger_device::in_w<5>));

	// COMM port 6 - 7
	NS16450(config, "uart_3a", MASTER_CLOCK / 9);
//  MCFG_INS8250_OUT_INT_CB(WRITELINE(*this, FUNC(input_merger_device::in_w<6>));
	NS16450(config, "uart_3b", MASTER_CLOCK / 9);
//  MCFG_INS8250_OUT_INT_CB(WRITELINE(*this, FUNC(input_merger_device::in_w<7>));

	INPUT_MERGER_ANY_HIGH(config, "uart_irq").output_handler().set(FUNC(aristmk5_state::uart_irq_callback));

	DS1302(config, m_rtc, 32.768_kHz_XTAL);

	HOPPER(config, m_hopper, attotime::from_msec(100), TICKET_MOTOR_ACTIVE_HIGH, TICKET_STATUS_ACTIVE_LOW);

	SPEAKER(config, "speaker").front_center();
	for (int i = 0; i < 8; i++)
	{
		DAC_16BIT_R2R_TWOS_COMPLEMENT(config, m_dac[i], 0).add_route(0, "speaker", 0.1); // unknown DAC
	}
	voltage_regulator_device &vref(VOLTAGE_REGULATOR(config, "vref", 0));
	vref.add_route(0, "dac0", 1.0, DAC_VREF_POS_INPUT); vref.add_route(0, "dac0", -1.0, DAC_VREF_NEG_INPUT);
	vref.add_route(0, "dac1", 1.0, DAC_VREF_POS_INPUT); vref.add_route(0, "dac1", -1.0, DAC_VREF_NEG_INPUT);
	vref.add_route(0, "dac2", 1.0, DAC_VREF_POS_INPUT); vref.add_route(0, "dac2", -1.0, DAC_VREF_NEG_INPUT);
	vref.add_route(0, "dac3", 1.0, DAC_VREF_POS_INPUT); vref.add_route(0, "dac3", -1.0, DAC_VREF_NEG_INPUT);
	vref.add_route(0, "dac4", 1.0, DAC_VREF_POS_INPUT); vref.add_route(0, "dac4", -1.0, DAC_VREF_NEG_INPUT);
	vref.add_route(0, "dac5", 1.0, DAC_VREF_POS_INPUT); vref.add_route(0, "dac5", -1.0, DAC_VREF_NEG_INPUT);
	vref.add_route(0, "dac6", 1.0, DAC_VREF_POS_INPUT); vref.add_route(0, "dac6", -1.0, DAC_VREF_NEG_INPUT);
	vref.add_route(0, "dac7", 1.0, DAC_VREF_POS_INPUT); vref.add_route(0, "dac7", -1.0, DAC_VREF_NEG_INPUT);
}


void aristmk5_state::aristmk5_touch(machine_config &config)
{
	aristmk5(config);
	subdevice<ns16450_device>("uart_0a")->out_tx_callback().set("microtouch", FUNC(microtouch_device::rx));

	microtouch_device &microtouch(MICROTOUCH(config, "microtouch", 2400));
	microtouch.stx().set("uart_0a", FUNC(ins8250_uart_device::rx_w));
}

void aristmk5_state::aristmk5_usa(machine_config &config)
{
	aristmk5(config);
	m_maincpu->set_addrmap(AS_PROGRAM, &aristmk5_state::aristmk5_usa_map);
}

void aristmk5_state::aristmk5_usa_touch(machine_config &config)
{
	aristmk5_usa(config);
	subdevice<ns16450_device>("uart_0a")->out_tx_callback().set("microtouch", FUNC(microtouch_device::rx));

	microtouch_device &microtouch(MICROTOUCH(config, "microtouch", 2400));
	microtouch.stx().set("uart_0a", FUNC(ins8250_uart_device::rx_w));
}

#define ARISTOCRAT_MK5_USA_SETCHIPS \
	ROM_REGION( 0x400000, "set_4.04.09", ROMREGION_ERASEFF ) /* setchip v4.04.09 4meg */ \
	ROM_LOAD32_WORD( "setchip v4.04.09.u7",  0x000000, 0x80000, CRC(e8e8dc75) SHA1(201fe95256459ce34fdb6f7498135ab5016d07f3) ) \
	ROM_LOAD32_WORD( "setchip v4.04.09.u11", 0x000002, 0x80000, CRC(ff7a9035) SHA1(4352c4336e61947c555fdc80c61f944076f64b64) ) \
	ROM_REGION( 0x400000, "set_4.04.08", ROMREGION_ERASEFF ) /* setchip v4.04.08 4meg (27c4096) */ \
	ROM_LOAD32_WORD( "setchip v4.04.08.u7",  0x000000, 0x80000, CRC(7c4b7fe4) SHA1(39dd39c794c0cb6abc1b7503650643a8131468d1) ) \
	ROM_LOAD32_WORD( "setchip v4.04.08.u11", 0x000002, 0x80000, CRC(d3234a28) SHA1(8ff112ee4aadf1d359ca8ffe0cfa9c7400aa0595) ) \
	ROM_REGION( 0x400000, "set_4.04.05", ROMREGION_ERASEFF ) /* setchip v4.04.05 4meg */ \
	ROM_LOAD32_WORD( "setchip v4.04.05.u7",  0x000000, 0x80000, CRC(e7b39a73) SHA1(e826d717a0871383394e15634896fcb2e2bdeb75) ) \
	ROM_LOAD32_WORD( "setchip v4.04.05.u11", 0x000002, 0x80000, CRC(2fc9b2a0) SHA1(89191f02c4ec8089e26989430806650d14e13e5a) ) \
	ROM_REGION( 0x400000, "set_4.04.00", ROMREGION_ERASEFF ) /* setchip v4.04.00 4meg 42pin */ \
	ROM_LOAD32_WORD( "setchip v4.04.00.u7",  0x000000, 0x80000, CRC(2453137e) SHA1(b59998e75ae3924da16faf47b9cfe9afd60d810c) ) \
	ROM_LOAD32_WORD( "setchip v4.04.00.u11", 0x000002, 0x80000, CRC(82dfa12a) SHA1(86fd0f0ad8d5d1bc503392a40bbcdadb055b2765) ) \
	ROM_REGION( 0x400000, "set_4.03.07", ROMREGION_ERASEFF ) /* setchip v4.03.07 4meg */ \
	ROM_LOAD32_WORD( "setchip v4.03.07.u7",  0x000000, 0x80000, CRC(5cf56036) SHA1(ff11deb6f76627f4fb5a7ec789e4071887e3eae9) ) \
	ROM_LOAD32_WORD( "setchip v4.03.07.u11", 0x000002, 0x80000, CRC(637de9b9) SHA1(4b5c974fd4c1d1a21f83bdc03d8bc5013d4a40ea) ) \
	ROM_REGION( 0x400000, "set_4.02.04", ROMREGION_ERASEFF ) /* setchip v4.02.04 */ \
	ROM_LOAD32_WORD( "setchip v4.02.04.u7",  0x000000, 0x80000, CRC(5a254b22) SHA1(8444f237b392df2a3cb42ea349e7af32f47dd544) ) \
	ROM_LOAD32_WORD( "setchip v4.02.04.u11", 0x000002, 0x80000, CRC(def36617) SHA1(c7ba5b08e884a8fb36c9fb51c08e243e32c81f89) ) \
	ROM_REGION( 0x400000, "set_1.0",     ROMREGION_ERASEFF ) /* setchip v1.0 4meg */ \
	ROM_LOAD32_WORD( "setchip v1.0.u7",      0x000000, 0x80000, CRC(16c2828d) SHA1(87218a76bb2791bce27b4b8ab341ba73cbbf6ffe) ) \
	ROM_LOAD32_WORD( "setchip v1.0.u11",     0x000002, 0x80000, CRC(a969f12b) SHA1(fc6d234ce1f5c6b5aceb97b73aad60144352023b) )

#define ARISTOCRAT_MK5_GALS \
	ROM_REGION( 0x600, "gals", 0 ) \
	ROM_LOAD( "a562837.u36",  0x000000, 0x000157, CRC(1f269234) SHA1(29940dd50fb55c632935f62ff44ca724379c7a43) ) \
	ROM_LOAD( "a562838.u65",  0x000200, 0x000157, CRC(f2f3c40a) SHA1(b795dfa5cc4e8127c3f3a0906664910d1325ec92) ) \
	ROM_LOAD( "a562840.u22",  0x000400, 0x000157, CRC(941d4cdb) SHA1(1ca091fba69e92f262dbb3d40f515703c8981793) )

#define ARISTOCRAT_MK5_EEPROM \
	ROM_REGION16_BE( 0x100, "eeprom0", ROMREGION_ERASEFF ) \
	ROM_REGION16_BE( 0x100, "eeprom1", ROMREGION_ERASEFF )


ROM_START( aristmk5 )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM

	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )

	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )

	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200751V / 602/9 / 10 Credit Multiplier / 20 Line Multiline
// ADONIS / NSW/ACT / A - 25/05/98
ROM_START( adonis )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000bf8
	    0x000000-0x05eb1b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xfc98a056
	        Calculated Checksum 0xfc98a056  (OK)
	    0x05eb1c-0x10fa8b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05eb1c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200751v.u7",  0x000000, 0x80000, CRC(ab386ab0) SHA1(56c5baea4272866a9fe18bdc371a49f155251f86) )
	ROM_LOAD32_WORD( "0200751v.u11", 0x000002, 0x80000, CRC(ce8c8449) SHA1(9894f0286f27147dcc437e4406870fe695a6f61a) )
	ROM_LOAD32_WORD( "0200751v.u8",  0x100000, 0x80000, CRC(99097a82) SHA1(a08214ab4781b06b46fc3be5c48288e373230ef4) )
	ROM_LOAD32_WORD( "0200751v.u12", 0x100002, 0x80000, CRC(443a7b6d) SHA1(c19a1c50fb8774826a1e12adacba8bbfce320891) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100751V / 602/9 / 10 Credit Multiplier / 20 Line Multiline
// ADONIS / NSW/ACT / A - 25/05/98
ROM_START( adonisa )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000bf8
	    0x000000-0x05cdc3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x91f374c7
	        Calculated Checksum 0x91f374c7  (OK)
	    0x05cdc4-0x11000b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05cdc4-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100751v.u7",  0x000000, 0x80000, CRC(ca3e97db) SHA1(bd0a4402e57891899d92ea85a87fb8925a44f706) )
	ROM_LOAD32_WORD( "0100751v.u11", 0x000002, 0x80000, CRC(cfe3f792) SHA1(aa1bf77101404c2018a5e5b808f1d683e29ae942) )
	ROM_LOAD32_WORD( "0100751v.u8",  0x100000, 0x80000, CRC(d55204bd) SHA1(208c089d435ea4af25d0b9b3d5e79fea397bc885) )
	ROM_LOAD32_WORD( "0100751v.u12", 0x100002, 0x80000, CRC(77090858) SHA1(76ebc15b26f378ac95276f0aa26d077e3646a6f1) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// BHG1508 / MV4124/1 / 5,10,25,50 Credit Multiplier / 20 Line Multiline
// ADONIS / Export / B - 31/07/01
// Requires set chips 4.04.xx
ROM_START( adonisu )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0e8a7b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xe6715f98
	        Calculated Checksum 0xe6715f98  (OK)
	    0x0e8a7c-0x1c5f47 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0e8a7c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "bhg1508.u7",  0x000000, 0x80000, CRC(09107577) SHA1(6cb525a96cdcb414ae7a45b5a5853d94059bc333) )
	ROM_LOAD32_WORD( "bhg1508.u11", 0x000002, 0x80000, CRC(45630329) SHA1(a2c8e3ae786948135797a80ae12bf3ba2e6192a8) )
	ROM_LOAD32_WORD( "bhg1508.u8",  0x100000, 0x80000, CRC(c572e39b) SHA1(e6ca864b620a17f8b963d4917cbdc894b9b275dc) )
	ROM_LOAD32_WORD( "bhg1508.u12", 0x100002, 0x80000, CRC(b8995b25) SHA1(ad39ad11dc02dbbe8be99d81c96e9fc3191726ad) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0201005V / 602/9 / 10 Credit Multiplier/20 Line Multiline
// ADONIS / NSW/ACT / C - 06/07/99
// Cash Express Hyperlink game
ROM_START( adonisce )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	/*
	    Checksum code found at 0x000c44
	    0x000000-0x06ddab is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x07c97aad
	        Calculated Checksum 0x07c97aad  (OK)
	    0x06ddac-0x2a41cb is the non-Checksummed range
	*/
	ROM_LOAD32_WORD( "0201005v.u7",  0x000000, 0x80000, CRC(32149323) SHA1(abfc6a8518a39528db3700c2cb558e925ceeda6d) )
	ROM_LOAD32_WORD( "0201005v.u11", 0x000002, 0x80000, CRC(309b0b55) SHA1(669568031d305b29395345a26a5d004d83881433) )
	ROM_LOAD32_WORD( "0201005v.u8",  0x100000, 0x80000, CRC(e9185e3c) SHA1(99609a152a55246d0f5377f943deec47e68fb9fc) )
	ROM_LOAD32_WORD( "0201005v.u12", 0x100002, 0x80000, CRC(8b675dff) SHA1(275579d21be51e6ca01be7c57018e142d1d40875) )
	ROM_LOAD32_WORD( "0201005v.u9",  0x200000, 0x80000, CRC(c2e973e7) SHA1(e89bdaa56b0c3c7bd77c8141421f76be9ff2e71b) )
	ROM_LOAD32_WORD( "0201005v.u13", 0x200002, 0x80000, CRC(e005a7e8) SHA1(cbb313f5d1d04c5a441b3f92b7a90a281ddb4885) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 01J02046 / JB013/1 / Multi credit / 20 line
// ALCHEMIST / VENEZUILA / A - 22/01/02
// This game is downported from the MK6 version (Alchemy) and has MK6 style graphics
// Venezuela is spelled as 'Venezuila' in the ROM
// Game is in Spanish, however audit mode is in English
ROM_START( alchemst )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000bb8
	    0x000000-0x08e937 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x04472e3b
	        Calculated Checksum 0x04472e3b  (OK)
	    0x08e938-0x2c839f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x08e938-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "01j02046.u7",  0x000000, 0x80000, CRC(1a315825) SHA1(d5390c13a6182fca6ca5eec7968a8be0af548468) )
	ROM_LOAD32_WORD( "01j02046.u11", 0x000002, 0x80000, CRC(1f21adea) SHA1(88a24ea08c476b880c3c8a0547442f065703c6c8) )
	ROM_LOAD32_WORD( "01j02046.u8",  0x100000, 0x80000, CRC(9fd79dc5) SHA1(510a45004cf760488977b7ac0ef79a04c3ec035f) )
	ROM_LOAD32_WORD( "01j02046.u12", 0x100002, 0x80000, CRC(fcd695fb) SHA1(28a6891f1dbaf919e8454f412090660bb604938e) )
	ROM_LOAD32_WORD( "01j02046.u9",  0x200000, 0x80000, CRC(3591eaf1) SHA1(e11c7ec630df69f7b7d507f3d28fc3530716f133) )
	ROM_LOAD32_WORD( "01j02046.u13", 0x200002, 0x80000, CRC(6b791adf) SHA1(f00923101f926034603243a3c63e1010b044829e) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200428V / 386/56 / CARD POKER
// BAD DOG POKER / NSW HOTEL / A 17/12/96
ROM_START( baddog )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ae4
	    0x000000-0x056f3f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x15ac4012
	        Calculated Checksum 0x15ac4012  (OK)
	    0x056f40-0x2fb607 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x056f40-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200428v.u7",  0x000000, 0x80000, CRC(25aa8109) SHA1(cf4521b3d447812d2d9dbfdab9fe0cec71cdeb2e) )
	ROM_LOAD32_WORD( "0200428v.u11", 0x000002, 0x80000, CRC(774ff977) SHA1(5ce1aa8b7598b4bc8e5fa44de1c03b5f2851f5de) )
	ROM_LOAD32_WORD( "0200428v.u8",  0x100000, 0x80000, CRC(e52a279a) SHA1(4a3a080d840d8a894ec0ba0250a566831377f0f8) )
	ROM_LOAD32_WORD( "0200428v.u12", 0x100002, 0x80000, CRC(562aa123) SHA1(825a2d23321b636a3ff2565b2b72df3b97bd0ec8) )
	ROM_LOAD32_WORD( "0200428v.u9",  0x200000, 0x80000, CRC(66d5a7f7) SHA1(1a1f845a97677c43ff1090231434ae9d3d36ab4c) )
	ROM_LOAD32_WORD( "0200428v.u13", 0x200002, 0x80000, CRC(883b2ec3) SHA1(5b431d8c9c8eabca65ab22dcf2bdb22d49445bb1) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200818V / 594/1 / 3 Credit Multiplier/3 Line Multiline
// Black Panther / Victoria / A - 30/07/96
ROM_START( blackpnt )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000bb0
	    0x000000-0x056d8b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xeebac434
	        Calculated Checksum 0xeebac434  (OK)
	    0x056d8c-0x138557 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x056d8c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	// the checksum only covers part of the first 2 roms, marked all as BAD_DUMP because it can't be trusted without a full redump.
	ROM_LOAD32_WORD( "0200818v.u7",  0x000000, 0x80000, BAD_DUMP CRC(eed76145) SHA1(6a40a6ba2ce320a37b086dc4916c92c8e38c065e) )
	ROM_LOAD32_WORD( "0200818v.u11", 0x000002, 0x80000, BAD_DUMP CRC(de3358d3) SHA1(4f290940d8af9fe8d404802d5cecfd2d098eff12) )
	ROM_LOAD32_WORD( "0200818v.u8",  0x100000, 0x80000, BAD_DUMP CRC(58ddfb50) SHA1(c2152e65fa119136b7944b69e650310db78e62a8) )
	ROM_LOAD32_WORD( "0200818v.u12", 0x100002, 0x80000, BAD_DUMP CRC(bb2bf7bb) SHA1(f88208238a69fc79e33af17f39e25cd2857d7172) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100812V / 616/1 / 25 Credit Multiplier/20 Line Multiline
// Boot Scootin' 500cm / NSW/ACT / B - 11/12/98
ROM_START( bootsctn )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    0x000000-0x06c177 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xb0980753
	        Calculated Checksum 0xb0980753  (OK)
	    0x06c178-0x384a9b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x06c178-0x3fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100812v.u7",  0x000000, 0x80000, CRC(f8e12462) SHA1(82a25757b2146204b86e557b8f1c45280e0668a8) )
	ROM_LOAD32_WORD( "0100812v.u11", 0x000002, 0x80000, CRC(df066d27) SHA1(310422c78e93ce9f1f58b4a58a59bc2eba5c502a) )
	ROM_LOAD32_WORD( "0100812v.u8",  0x100000, 0x80000, CRC(08e8de8d) SHA1(913d3e51821d8885affd2750c18d1000629b79d9) )
	ROM_LOAD32_WORD( "0100812v.u12", 0x100002, 0x80000, CRC(87ddc7ef) SHA1(91473d8fd266a909fa8d4ec3df3a61861c6e9f4c) )
	ROM_LOAD32_WORD( "0100812v.u9",  0x200000, 0x80000, CRC(a1ca5f2b) SHA1(c8fc6aff0c3819370339143966ec76910e40c671) )
	ROM_LOAD32_WORD( "0100812v.u13", 0x200002, 0x80000, CRC(fca82ee7) SHA1(bb70f2e04047a58b697dca536b95f9bbcc295a8a) )
	ROM_LOAD32_WORD( "0100812v.u10", 0x300000, 0x80000, CRC(b574c12d) SHA1(3b1d1d00ef3eae23493e2b0381ab80490af510d4) )
	ROM_LOAD32_WORD( "0100812v.u14", 0x300002, 0x80000, CRC(75b9b89e) SHA1(08d487b3722f2ea5d2d18c78f571a44c78616dbe) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// MV4098 / 10 Credit Multiplier / 9 Line Multiline
// BOOT SCOOTIN' / Export / A - 25/08/99
// All devices are 27c4002 instead of 27c4096
// Marked as GHG101202 and 92.767%
// No set chips required
ROM_START( bootsctnu )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0941ab is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xdf68cecf
	        Calculated Checksum 0xdf68cecf  (OK)
	    0x0941ac-0x328187 is the non-Checksummed range (unusual endpoint)
	*/

	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ghg101202.u7",  0x000000, 0x80000, CRC(ea389201) SHA1(a838a9115ca3101cb06ec86e955294d51f8d65e4) )  // 92.767%
	ROM_LOAD32_WORD( "ghg101202.u11", 0x000002, 0x80000, CRC(af6f2142) SHA1(200be993ad21643c46995d68a119fd8c1d2fedbe) )  // 92.767%
	ROM_LOAD32_WORD( "ghg101202.u8",  0x100000, 0x80000, CRC(7b9c153e) SHA1(2144a208d2fd2f993e113d809045e864d89df7ec) )  // base
	ROM_LOAD32_WORD( "ghg101202.u12", 0x100002, 0x80000, CRC(565ce1ca) SHA1(a9a64027d78f9ca7036b5b299040e1a7c776b617) )  // base
	ROM_LOAD32_WORD( "ghg101202.u9",  0x200000, 0x80000, CRC(b60ea737) SHA1(847cdb4a6bf887a08dd525320aeffc74fcc10643) )  // base
	ROM_LOAD32_WORD( "ghg101202.u13", 0x200002, 0x80000, CRC(55ddba7b) SHA1(3a43e577e8bc72aa1c09b131ec8b2a87eaeccf9f) )  // base
	ROM_LOAD32_WORD( "ghg101202.u10", 0x300000, 0x80000, CRC(230725bc) SHA1(85320b1b72c7f83f57bc8cc861e5a6323750ff4f) )  // base
	ROM_LOAD32_WORD( "ghg101202.u14", 0x300002, 0x80000, CRC(18934c51) SHA1(f7c9c95c687dbfe89747e7877157fde37bc1119e) )  // base

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// GHG1008-03 / MV4098/1 / 10 Credit Multiplier/20 Line Multiline
// BOOT SCOOTIN' / Export / A - 27/07/99
ROM_START( bootsctnua )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0944bf is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xb79e9367
	        Calculated Checksum 0xb79e9367  (OK)
	    0x0944c0-0x32849b is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ghg100803.u7",  0x000000, 0x80000, CRC(56749bb5) SHA1(391e2cc4e17c56c9c3a40dba34970b606cf7e452) ) // 94.858%
	ROM_LOAD32_WORD( "ghg100803.u11", 0x000002, 0x80000, CRC(3a38fec2) SHA1(1a4171bf40368f38bf93323daa640da9220f23a4) ) // 94.858%
	ROM_LOAD32_WORD( "ghg100803.u8",  0x100000, 0x80000, CRC(85cf7289) SHA1(500d236bdf82a2ef37919c2756ec4695729e9d15) )
	ROM_LOAD32_WORD( "ghg100803.u12", 0x100002, 0x80000, CRC(c44e560c) SHA1(c638850afddbfc6e8d89e077ed54624543a10e33) )
	ROM_LOAD32_WORD( "ghg100803.u9",  0x200000, 0x80000, CRC(ac3d6eeb) SHA1(0a5ded18b8b5ed2d8bfc98ab5efe2564a0e5a0d0) )
	ROM_LOAD32_WORD( "ghg100803.u13", 0x200002, 0x80000, CRC(5ef50865) SHA1(07bd31fab356142e548f6aa27d15ed5646064f15) )
	ROM_LOAD32_WORD( "ghg100803.u10", 0x300000, 0x80000, CRC(bebc7aaa) SHA1(3b63ba76a96677032776e17761ed281541f94513) )
	ROM_LOAD32_WORD( "ghg100803.u14", 0x300002, 0x80000, CRC(9759692e) SHA1(7666027e21af27329720127367a780776973c515) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1547 / MV4098/1 / 10 Credit Multiplier / 20 Line Multiline
// BOOT SCOOTIN' / Export / B - 22/03/01
// Requires set chip version: 4.04.xx
ROM_START( bootsctnub )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0f47a7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xae44f291
	        Calculated Checksum 0xae44f291  (OK)
	    0x0f47a8-0x3a0b5b is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1547.u7",  0x000000, 0x80000, CRC(5ded48cb) SHA1(3335a9dc6758dd8b225258ab2cb01cf4e9c02dd2) )
	ROM_LOAD32_WORD( "ahg1547.u11", 0x000002, 0x80000, CRC(764dfdb8) SHA1(e68493dfac2d64c59d7576d4b124210089edda62) )
	ROM_LOAD32_WORD( "ahg1547.u8",  0x100000, 0x80000, CRC(7e26e7a9) SHA1(88dc5dbf6ada1b6f2218ef3b013bc91047401d21) )
	ROM_LOAD32_WORD( "ahg1547.u12", 0x100002, 0x80000, CRC(f2dfc38c) SHA1(7fd6c8e11feef375b0f1fac0cfebca74f5da2ed6) )
	ROM_LOAD32_WORD( "ahg1547.u9",  0x200000, 0x80000, CRC(13f410e4) SHA1(e5022dbb42d174b2e3eda8f1564c97ca8fa39deb) )
	ROM_LOAD32_WORD( "ahg1547.u13", 0x200002, 0x80000, CRC(a5382c96) SHA1(570479383d69dfc9e52ebd55b51989ca284d31e7) )
	ROM_LOAD32_WORD( "ahg1547.u10", 0x300000, 0x80000, CRC(a31d8a45) SHA1(5e62d0d3523fd381e940b4dbc6f84d83434bf83a) )
	ROM_LOAD32_WORD( "ahg1547.u14", 0x300002, 0x80000, CRC(de2888e2) SHA1(f614d68c0e09912e9126cb024f54ed32ee50b57e) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// BHG1248 / MV4119/1 / 3,5,10,20,25,50 Credit Multiplier / 9 Line Multiline
// Bachelorette Party / Export / B - 25/08/2000
// ROM says '9 Line Multiline' but this is a 20 line game, it cannot be set to 9 lines at all
// Touchscreen game
// Game requires set chip version: 4.01.xx
ROM_START( bparty )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0a693f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x8eb73e23
	        Calculated Checksum 0x8eb73e23  (OK)
	    0x0a6940-0x39cda7 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "bhg1248.u7",  0x000000, 0x80000, CRC(6e432a78) SHA1(3505cf255f63365e5cc7c1e8338509a2889b99be) )
	ROM_LOAD32_WORD( "bhg1248.u11", 0x000002, 0x80000, CRC(c9244e66) SHA1(5ea15951c4e003378549c2a581c32564327bd3bf) )
	ROM_LOAD32_WORD( "bhg1248.u8",  0x100000, 0x80000, CRC(344c4061) SHA1(6041a8198e82416af48131f2e1bb59341e99e365) )
	ROM_LOAD32_WORD( "bhg1248.u12", 0x100002, 0x80000, CRC(79034324) SHA1(be833ea47a8f9abc1415d14c1d499572a64b2374) )
	ROM_LOAD32_WORD( "bhg1248.u9",  0x200000, 0x80000, CRC(ecc5b6a2) SHA1(2d8d0b000600f0ae965fe0ff53a7f7e5c169a0a7) )
	ROM_LOAD32_WORD( "bhg1248.u13", 0x200002, 0x80000, CRC(dade5590) SHA1(e604a87aeb5284daec2a35c395ef52213b16da7d) )
	ROM_LOAD32_WORD( "bhg1248.u10", 0x300000, 0x80000, CRC(fc6310db) SHA1(1a7f31f884c4b2838edaffd13c212b887d218592) )
	ROM_LOAD32_WORD( "bhg1248.u14", 0x300002, 0x80000, CRC(b276d61a) SHA1(8bee7fa551caec3da03afa061612c153f7b48cdb) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// BHG1579 / MV4119/1 / 5, 10, 25, 50 Credit Multiplier / 20 Line Multiline
// Bachelorette Party / Export / B - 25/08/2000
// Touchscreen game
ROM_START( bpartya )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to ROMs being corrupted, all files are missing bytes consisting of 0x0D
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "bhg1579.u7",  0x000000, 0x7f01b, BAD_DUMP CRC(da30ade2) SHA1(0a19181ae3968134a5731aa9eadde8c7a12798c1) )
	ROM_LOAD32_WORD( "bhg1579.u11", 0x000002, 0x7ff5b, BAD_DUMP CRC(f94c777f) SHA1(f3e516b9d8b0270f7935bf80f7fabfef055171f3) )
	ROM_LOAD32_WORD( "bhg1579.u8",  0x100000, 0x7fe04, BAD_DUMP CRC(9f457ac5) SHA1(913b48a5d49c555dfa758aee619d32bf32daf761) )
	ROM_LOAD32_WORD( "bhg1579.u12", 0x100002, 0x7fdfe, BAD_DUMP CRC(d18929d8) SHA1(0f1d9c8b48a2f157ec8447bff08815e2ad15782c) )
	ROM_LOAD32_WORD( "bhg1579.u9",  0x200000, 0x7fd8e, BAD_DUMP CRC(08c95d7f) SHA1(ad37b96e3474bac7e06156c19b08a0ae313d7e42) )
	ROM_LOAD32_WORD( "bhg1579.u13", 0x200002, 0x7fd9c, BAD_DUMP CRC(9f0f893d) SHA1(027048eef5f791a4e051692108288ee4b12152e6) )
	ROM_LOAD32_WORD( "bhg1579.u10", 0x300000, 0x7ff63, BAD_DUMP CRC(b2682002) SHA1(378acb39ae504423506b3a01b1ba91d1e8ae0be0) )
	ROM_LOAD32_WORD( "bhg1579.u14", 0x300002, 0x7ff94, BAD_DUMP CRC(34ffe312) SHA1(34432e57e2e3dd90c15dd3ff7cb16d8381343be8) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200510V / 593 / 10 Credit Multiplier / 9 Line Multiline
// Bumble Bugs / Local / D - 5/07/96
ROM_START( bumblbug )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x05b94b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xf5d418fe
	        Calculated Checksum 0xf5d418fe  (OK)
	    0x05b94c-0x0fc69f is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200510v.u7",  0x000000, 0x80000, CRC(d4cfce73) SHA1(735c385779afe55e521dbfe9ebfdc55fe3346349) )
	ROM_LOAD32_WORD( "0200510v.u11", 0x000002, 0x80000, CRC(5d888245) SHA1(bbbe61e09bebd5fcb79f060d5caee15100c9a685) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200546V / 593 / 10 Credit Multiplier / 9 Line Multiline
// Bumble Bugs / QLD CLUB & HOTEL / D - 05/07/96
ROM_START( bumblbugql )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ac8
	    0x000000-0x05554b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x66b20ae6
	        Calculated Checksum 0x66b20ae6  (OK)
	    0x05554c-0x1c4e2b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05554c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200456v.u7",  0x000000, 0x80000, CRC(f04dd78b) SHA1(443057fc3e02406d46cf68f95e85e5a0fd8e7b1e) )
	ROM_LOAD32_WORD( "0200456v.u11", 0x000002, 0x80000, CRC(3b50b21b) SHA1(7c20d1bfb82cdd19c046a545ae251e3560b8f00d) )
	ROM_LOAD32_WORD( "0200456v.u8",  0x100000, 0x80000, CRC(da86d682) SHA1(b1aa739215f1f0967d6a03060d9a2f10115c0b03) )
	ROM_LOAD32_WORD( "0200456v.u12", 0x100002, 0x80000, CRC(9f0d7615) SHA1(1453b1476510e1dd68bc14feba72dc59b9dfe676) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// CHG0479-03 / 593 / 10 Credit Multiplier / 9 Line Multiline
// Bumble Bugs / Export / D - 05/07/97
// All devices are 27c4002 instead of 27c4096
// Marked as CHG047903 and 92.691%
// No set chips required
ROM_START( bumblbugu )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0b1f47 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x9f3936f9
	        Calculated Checksum 0x9f3936f9  (OK)
	    0x0b1f48-0x183c1f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0b1f48-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "chg047903.u7",  0x000000, 0x80000, CRC(ae49d394) SHA1(fae7db8193915a3307e89a4623289fcbf9486283) )  // 92.691%
	ROM_LOAD32_WORD( "chg047903.u11", 0x000002, 0x80000, CRC(6da81e81) SHA1(a9e7aff23b24213ce2c68f672a9e22088cf53f3f) )  // 92.691%
	ROM_LOAD32_WORD( "chg047903.u8",  0x100000, 0x80000, CRC(e0c01d01) SHA1(9153129fd348a97da7cccf002e5d03e4b4db9264) )  // base
	ROM_LOAD32_WORD( "chg047903.u12", 0x100002, 0x80000, CRC(7dbb634b) SHA1(f7a752240989032af142f4cd6e4260a12a5b4c0a) )  // base

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// CHG0479-99 / 593 / 10 Credit Multiplier / 9 Line Multiline
// Bumble Bugs / Export / D - 05/07/97
// All devices are 27c4002 instead of 27c4096
// Marked as CHG047999 and 87.006%
// Variation (% and NO): 87.006% 99
// No set chips required
ROM_START( bumblbugua )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0b1f47 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xa23f49f6
	        Calculated Checksum 0xa23f49f6  (OK)
	    0x0b1f48-0x183c1f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0b1f48-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "chg047999.u7",  0x000000, 0x80000, CRC(7e9be2d0) SHA1(84293a391e9797afc904bb3be05367361ae8d4c7) )  // 87.006%
	ROM_LOAD32_WORD( "chg047999.u11", 0x000002, 0x80000, CRC(efd350ac) SHA1(19975b4d2b55b65542a7ff38fa035fd516bd6e2e) )  // 87.006%
	ROM_LOAD32_WORD( "chg047999.u8",  0x100000, 0x80000, CRC(e0c01d01) SHA1(9153129fd348a97da7cccf002e5d03e4b4db9264) )  // base
	ROM_LOAD32_WORD( "chg047999.u12", 0x100002, 0x80000, CRC(7dbb634b) SHA1(f7a752240989032af142f4cd6e4260a12a5b4c0a) )  // base

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200143V / 571/4 / 10 Credit Multiplier/9 Line Multiline
// Butterfly Delight / Local / A - 19/12/95
ROM_START( buttdeli )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000adc
	    0x000000-0x04477f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x19143954
	        Calculated Checksum 0x19143954  (OK)
	    0x044780-0x1c713b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x044780-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200143v.u7",  0x000000, 0x80000, CRC(7f69cdfc) SHA1(1241741d21334df10d60080555824a87eae93db3) )
	ROM_LOAD32_WORD( "0200143v.u11", 0x000002, 0x80000, CRC(1ddf8732) SHA1(dc09db14c251699fdd46068f18ad6214e8752939) )
	ROM_LOAD32_WORD( "0200143v.u8",  0x100000, 0x80000, CRC(24d8135e) SHA1(1bc69e9927afe0300d15a49ca06ae527774b295a) )
	ROM_LOAD32_WORD( "0200143v.u12", 0x100002, 0x80000, CRC(0d58cf28) SHA1(aa65b7ee88b5bc872008a46e60bd49d9e5eda153) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1463 / 603(a) / 3,5,10,25,50 Credit Multiplier / 20 Line Multiline
// Canyon Rose 100cm / Export / B - 06/12/96
ROM_START( canrose )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to ROMs being corrupted, all files are missing bytes consisting of 0x0D
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1463.u7",  0x000000, 0x7f06d, CRC(d866097c) SHA1(2bd2c6200986b27a35329aa0c43e5afd22becbfc) )
	ROM_LOAD32_WORD( "ahg1463.u11", 0x000002, 0x7ff68, CRC(710827f7) SHA1(26b9ab7f49dc94467a98635480cac0605c1de399) )
	ROM_LOAD32_WORD( "ahg1463.u8",  0x100000, 0x7f499, CRC(17e4ff76) SHA1(e582c92478f139e55922ccf851e4922078498462) )
	ROM_LOAD32_WORD( "ahg1463.u12", 0x100002, 0x7f4fc, CRC(5fe736c2) SHA1(d7c1a3f003085848e413aa499d9eaecca74773da) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100676V / 614/3 / 10 Credit Multiplier/20 Line Multiline
// Cash Cat 200cm / NSW/ACT / A - 3/04/98
ROM_START( cashcat )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x0615f7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x7b4c06fa
	        Calculated Checksum 0x7b4c06fa  (OK)
	    0x0615f8-0x1fffef is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0615f8-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_LOAD32_WORD( "0100676v.u7",  0x000000, 0x80000, CRC(5c3a3805) SHA1(b94a400d1da316cb25adc8e2691f9d4d577f7104) )
	ROM_LOAD32_WORD( "0100676v.u11", 0x000002, 0x80000, CRC(7cdd3933) SHA1(db191eabde61345ecd9528790bb78484b243c5f3) )
	ROM_LOAD32_WORD( "0100676v.u8",  0x100000, 0x80000, CRC(87a8d9a9) SHA1(93ad5a0f3579845e187c5a5a45e6bdc476cd4d89) )
	ROM_LOAD32_WORD( "0100676v.u12", 0x100002, 0x80000, CRC(a7199f5f) SHA1(6a46935c095b1d89307921e3a53b48032e6f45fa) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100557V / 614/1 / 10 Credit Multiplier/9 Line Multiline
// Cash Cat 90cm / NSW/ACT / B - 1/12/97
ROM_START( cashcata )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x0612df is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xdd9daebd
	        Calculated Checksum 0xdd9daebd  (OK)
	    0x0612e0-0x18796b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0612e0-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_LOAD32_WORD( "0100557v.u7",  0x000000, 0x80000, CRC(441baf3a) SHA1(0770b2b9119cc528806a910c25090649f9f0f9a5) )
	ROM_LOAD32_WORD( "0100557v.u11", 0x000002, 0x80000, CRC(a67962e4) SHA1(2436d8028b739bbccf757344ef67a60dca79e81b) )
	ROM_LOAD32_WORD( "0100557v.u8",  0x100000, 0x80000, CRC(9e07de68) SHA1(455f912e10517867e938f0b3ce63ff1e3a14ca1d) )
	ROM_LOAD32_WORD( "0100557v.u12", 0x100002, 0x80000, CRC(bdeeafd3) SHA1(a95a44ff8534bb030d696a37821f3e53072f2947) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0300863V / MV4089 / 5 Credit Multiplier/9 Line Multiline
// Cash Cat / New Zealand / A- 4/1/99
ROM_START( cashcatnz )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	// checksum code not found (uses different startup sequence)
	ROM_LOAD32_WORD( "0300863v.u7",  0x000000, 0x80000, CRC(de0f0202) SHA1(994f6c47b1e2e0e133853dc69b189752104486e4) )
	ROM_LOAD32_WORD( "0300863v.u11", 0x000002, 0x80000, CRC(e60e8bd1) SHA1(ffaa7be8968047b9ee54a117d273a14cbca41028) )
	ROM_LOAD32_WORD( "0300863v.u8",  0x100000, 0x80000, CRC(37d41d35) SHA1(c959b787383d6f91d20e18f37a38a965407a9ff0) )
	ROM_LOAD32_WORD( "0300863v.u12", 0x100002, 0x80000, CRC(f930fc07) SHA1(cb3fdbd5b87af7b14067f7999740470d3cf434df) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100438V / 603/1 / 25 Credit Multiplier / 20 Line Multiline
// Cash Chameleon / NSW/ACT / C - 15/4/97
ROM_START( cashcham )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ae0
	    0x000000-0x055f83 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x159a2aa3
	        Calculated Checksum 0x159a2aa3  (OK)
	    0x055f84-0x1dbdd7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x055f84-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100438v.u7",  0x000000, 0x80000, CRC(c942ef22) SHA1(4f56674f749602ae928832f98a641e680af8989b) )
	ROM_LOAD32_WORD( "0100438v.u11", 0x000002, 0x80000, CRC(64921874) SHA1(5aa6a0d6e29f5e400e275f27b6adfbef595fe83a) )
	ROM_LOAD32_WORD( "0100438v.u8",  0x100000, 0x80000, CRC(a8868277) SHA1(e199448a0a920219dc15443813061653b94d6d3a) )
	ROM_LOAD32_WORD( "0100438v.u12", 0x100002, 0x80000, CRC(7ae3b5db) SHA1(238698b72f529ac4fb292d08267069d1da01b43b) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200437V / 603(a) / 5 Credit Multiplier / 20 Line Multiline
// Cash Chameleon 100cm / NSW/ACT / D - 18/02/98
ROM_START( cashchama )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b00
	    0x000000-0x05ca1b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xa32ccd1b
	        Calculated Checksum 0xa32ccd1b  (OK)
	    0x05ca1c-0x1dbdd7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05ca1c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200437v.u7",  0x000000, 0x80000, CRC(a287fd5a) SHA1(7d06f679e5ff38e0989819410856361962c93e42) )
	ROM_LOAD32_WORD( "0200437v.u11", 0x000002, 0x80000, CRC(1875532b) SHA1(e410524b94b1c7860c1ef81ce5e0b4bf992f12ad) )
	ROM_LOAD32_WORD( "0200437v.u8",  0x100000, 0x80000, CRC(edbfc684) SHA1(8849374e5df34359d228a4b447c409b76fe36b35) )
	ROM_LOAD32_WORD( "0200437v.u12", 0x100002, 0x80000, CRC(571aab82) SHA1(03895d1a08d2dd868fd594db1aaeb29b295f0d98) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0300781V / MV4067 / 5 Credit Multiplier/20 Line Multiline
// Cash Chameleon / New Zealand / A - 31/08/98
ROM_START( cashchamnz )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found (uses different startup sequence)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0300781v.u7",  0x000000, 0x80000, CRC(009e109e) SHA1(b912b474a226af17bef554f4db6fade7cd2e558f) )
	ROM_LOAD32_WORD( "0300781v.u11", 0x000002, 0x80000, CRC(826da4ac) SHA1(6bf852b438f5257474c265ace2826b7bd0d9b087) )
	ROM_LOAD32_WORD( "0300781v.u8",  0x100000, 0x80000, CRC(f798ab06) SHA1(0f51ffd0e7abee6af0c5a29ab9ad1c8bfcd567a0) )
	ROM_LOAD32_WORD( "0300781v.u12", 0x100002, 0x80000, CRC(2aeb0265) SHA1(50e526ecccfdd35f7e156e1873cf4c81fb117069) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// DHG4078 / 603(a) / 3,5,10,25,50 Credit Multiplier / 20 Line Multiline
// Cash Chameleon 100cm / Export / B - 06/12/96
// Marked as DHG4078.
// Game requires set chip version: 4.00.xx
ROM_START( cashchamu )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x09b413 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x741cd9a0
	        Calculated Checksum 0x741cd9a0  (OK)
	    0x09b414-0x1b550b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x09b414-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "dhg407899.u7",  0x000000, 0x80000, CRC(9944fe04) SHA1(5f68b08e3480c0c8c1a29fe40e0221e9aeb969b7) )
	ROM_LOAD32_WORD( "dhg407899.u11", 0x000002, 0x80000, CRC(1f7fa88a) SHA1(6ca40c5edba758084fd4d4cc0899584fd2d72ad3) )
	ROM_LOAD32_WORD( "dhg407899.u8",  0x100000, 0x80000, CRC(6630b35b) SHA1(0126be7a73460fc70c7f1527a3eb0c9a0c58618e) )
	ROM_LOAD32_WORD( "dhg407899.u12", 0x100002, 0x80000, CRC(2fd087f8) SHA1(f16414abf58e1a7a8ca08380993bdeca6f9c7317) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0300467V / 607 / 10 Credit Multiplier/20 Line Multiline
// Cash Crop / Local / C - 14/07/97
ROM_START( cashcra5 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b80
	    0x000000-0x06076b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x2c872d3e
	        Calculated Checksum 0x2c872d3e  (OK)
	    0x06076c-0x1a2ecf is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x06076c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0300467v.u7",  0x000000, 0x80000, CRC(b0ff2aae) SHA1(b05667ffe952cae7a6581398552db6e47921090e) )
	ROM_LOAD32_WORD( "0300467v.u11", 0x000002, 0x80000, CRC(25a18efa) SHA1(0ee4f6cc66322397dbde53af2149f5fb35d788df) )
	ROM_LOAD32_WORD( "0300467v.u8",  0x100000, 0x80000, CRC(d4e7b4ba) SHA1(147a1ed5cdcbb84466a8024ad7e0778f85374489) )
	ROM_LOAD32_WORD( "0300467v.u12", 0x100002, 0x80000, CRC(570c7f8a) SHA1(7c9527e0b37970b7960c723727c3c650a48e8125) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0300447V / 607/2 / 10 Credit Multiplier/9 Line Multiline
// Cash Crop / Local / C - 29/08/97
ROM_START( cashcra5a )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to ROMs being corrupted, all files are missing bytes consisting of 0x0D
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0300447v.u7",  0x000000, 0x7f992, BAD_DUMP CRC(421ac2af) SHA1(552e98a0d3f969d702dd0aafcb4cb8f697a56b47) )
	ROM_LOAD32_WORD( "0300447v.u11", 0x000002, 0x7ffd3, BAD_DUMP CRC(36b57080) SHA1(6719df7cb0ae2535e125c965a2426efc00da29df) )
	ROM_LOAD32_WORD( "0300447v.u8",  0x100000, 0x7fe3d, BAD_DUMP CRC(9cc0ea01) SHA1(92f89d3adf257eee9ffaa88c1119f0456cafba1d) )
	ROM_LOAD32_WORD( "0300447v.u12", 0x100002, 0x7fe36, BAD_DUMP CRC(ef641efa) SHA1(52e54ed933352cde0f280ba2b3e9bae01c4aae7e) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100787V / 630/1 / 10 Credit Multiplier / 20 Line Multiline
// The Chariot Challenge / NSW/ACT / A - 10/08/98
ROM_START( chariotc )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ba8
	    0x000000-0x0603fb is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xbe63efe6
	        Calculated Checksum 0xbe63efe6  (OK)
	    0x0603fc-0x17a75b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0603fc-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100787v.u7",  0x000000, 0x80000, CRC(845f9913) SHA1(df6121290b30ff4a9c2d0e690cf8e7797e9a8612) )
	ROM_LOAD32_WORD( "0100787v.u11", 0x000002, 0x80000, CRC(bcbf9de9) SHA1(191ce749fe0d29b2783fb78d9338a00d65104daa) )
	ROM_LOAD32_WORD( "0100787v.u8",  0x100000, 0x80000, CRC(a3a74ecb) SHA1(52b3a41573a9fa1de05ce01a858e400f80e595b8) )
	ROM_LOAD32_WORD( "0100787v.u12", 0x100002, 0x80000, CRC(b44cf571) SHA1(04447820e015425493cade5611b3eb2f21e48c2e) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 04J00714 / 630 / 10 Credit Multiplier / 9 Line Multiline
// The Chariot Challenge / NSW/ACT / A - 10/08/98
// ROM contains unaltered NSW/ACT region string and date, but game is for the Venezuelan market
// Game is in Spanish, however audit mode is in English
ROM_START( chariotcv )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ba8
	    0x000000-0x07dbb7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x203ac6e8
	        Calculated Checksum 0x203ac6e8  (OK)
	    0x07dbb8-0x1b3787 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x07dbb8-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "04j00714.u7",  0x000000, 0x80000, CRC(2f3a1af7) SHA1(e1448116a81687cb79dd380dfbc8decf1f83e649) )
	ROM_LOAD32_WORD( "04j00714.u11", 0x000002, 0x80000, CRC(ef4f49e8) SHA1(8ff21f679a55cdfebcf22c109dfd3b41773293bd) )
	ROM_LOAD32_WORD( "04j00714.u8",  0x100000, 0x80000, CRC(fa24cfde) SHA1(1725c38a8a15915d8aa8e59afef9ce1d6e8d01c5) )
	ROM_LOAD32_WORD( "04j00714.u12", 0x100002, 0x80000, CRC(b8d4a5ec) SHA1(097e44cdb30b9aafd7f5358c8f0cdd130ec0615e) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 01J00681 / JB011 / Multi credit / Multi line
// Ckeckmate / NSW/ACT / B - 06/07/01
// Checkmate is misspelled as 'Ckeckmate' in the ROM
ROM_START( checkma5 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000c38
	    0x000000-0x071847 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x0de9b6ca
	        Calculated Checksum 0x0de9b6ca  (OK)
	    0x071848-0x25ff4b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x071848-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "01j00681.u7",  0x000000, 0x80000, CRC(059b940e) SHA1(f637508dafbd37169429c495a893addbc6d28834) )
	ROM_LOAD32_WORD( "01j00681.u11", 0x000002, 0x80000, CRC(5fb7bfb3) SHA1(2ad8b3c4753d19f9e3254ef3f4059951d7a111b4) )
	ROM_LOAD32_WORD( "01j00681.u8",  0x100000, 0x80000, CRC(6912cc4a) SHA1(9469a6a0d2fd39d85655a8c7bc0668752f5f11fa) )
	ROM_LOAD32_WORD( "01j00681.u12", 0x100002, 0x80000, CRC(b538bcbc) SHA1(cda404f9b16e7e76a33c208f62a5ac9c5e02aac4) )
	ROM_LOAD32_WORD( "01j00681.u9",  0x200000, 0x80000, CRC(53a573f0) SHA1(d51d698dcec273d157319200ad1c215e930b96ce) )
	ROM_LOAD32_WORD( "01j00681.u13", 0x200002, 0x80000, CRC(ad12a718) SHA1(0c36729cb8da800668f533f65fcc870f5dfc0f6a) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100351V / 596 / 10 Credit Multiplier/9 Line Multiline
// Chicken / Local / A - 27/08/96
ROM_START( chickna5 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b80
	    0x000000-0x053fb7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x8afbaabc
	        Calculated Checksum 0x8afbaabc  (OK)
	    0x053fb8-0x2fda37 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100351v.u7",  0x000000, 0x80000, CRC(be69c21c) SHA1(8b546727b5972f33d077db0a64aa41a7fde6d417) )
	ROM_LOAD32_WORD( "0100351v.u11", 0x000002, 0x80000, CRC(65423867) SHA1(992bb4f717f79233d1300d248b145f95a627cff2) )
	ROM_LOAD32_WORD( "0100351v.u8",  0x100000, 0x80000, CRC(3161c16f) SHA1(8f2b14ec8ba5c9da80a226d2ce5a7e5256c8cbb4) )
	ROM_LOAD32_WORD( "0100351v.u12", 0x100002, 0x80000, CRC(77b5d777) SHA1(f03afeaff08c9216e714f1e4bcc50292ba87ace4) )
	ROM_LOAD32_WORD( "0100351v.u9",  0x200000, 0x80000, CRC(5506777b) SHA1(42512577056e1caefbea0e74879780c56787af13) )
	ROM_LOAD32_WORD( "0100351v.u13", 0x200002, 0x80000, CRC(88a1ccae) SHA1(e242f48f99044b4fdf1bf36d8e105df09f94aa50) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200530V / 596 / 10 Credit Multiplier/9 Line Multiline
// Chicken / QLD Club & Hotels / C - 23/02/98
ROM_START( chickna5ql )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ac8
	    0x000000-0x05f193 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xeff4424a
	        Calculated Checksum 0xeff4424a  (OK)
	    0x05f194-0x3a9a7f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05f194-0x3fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200530v.u7",  0x000000, 0x80000, CRC(2d53de96) SHA1(6f2ed8f68d0474021a302d7e06ba869c0f1f7262) )
	ROM_LOAD32_WORD( "0200530v.u11", 0x000002, 0x80000, CRC(ed80acab) SHA1(d7ec3a063c45180e0b32935db9b8a01bcdaaa9a7) )
	ROM_LOAD32_WORD( "0200530v.u8",  0x100000, 0x80000, CRC(fbe704d3) SHA1(fe06489ba9628307f54ab60ab6909b45491116ae) )
	ROM_LOAD32_WORD( "0200530v.u12", 0x100002, 0x80000, CRC(c78215c0) SHA1(52b372df4a0f78cc557a874d8d40819aed191cdd) )
	ROM_LOAD32_WORD( "0200530v.u9",  0x200000, 0x80000, CRC(835903f5) SHA1(0a3bbc4e81629265d873716e9120eb95ea28b42c) )
	ROM_LOAD32_WORD( "0200530v.u13", 0x200002, 0x80000, CRC(7a5c1ca3) SHA1(d56103142392234298117d6b0d9163e0d3e52a7e) )
	ROM_LOAD32_WORD( "0200530v.u10", 0x300000, 0x80000, CRC(6c9399c1) SHA1(066afebc8ffcdf9e6a176e18997db242aa84269b) )
	ROM_LOAD32_WORD( "0200530v.u14", 0x300002, 0x80000, CRC(e87cf6c9) SHA1(a330644658da6100d7231b1c47260dc7f2e88448) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// RHG0730-03 / 596 / 10 Credit Multiplier / 9 Line Multiline
// Chicken / Export / C - 23/02/98
// Marked as RHG0730, 92.588%
// All devices are 27c4002 instead of 27c4096.
// (the following notes are from another identical set)
// Game Eprom: RHG073003
// No set chips required
// Variation (% and NO): 92.588% 03
ROM_START( chickna5u )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0a6917 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x0d44c6b0
	        Calculated Checksum 0x0d44c6b0  (OK)
	    0x0a6918-0x35040b is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "rhg073003.u7",  0x000000, 0x80000, CRC(06558129) SHA1(be726c0d35776faf1ecd20eb0a193e68a1fb1a84) )
	ROM_LOAD32_WORD( "rhg073003.u11", 0x000002, 0x80000, CRC(0eadf5d4) SHA1(b783f6e1911fc098d1b4d1d8c75862e031078e5b) )
	ROM_LOAD32_WORD( "rhg073003.u8",  0x100000, 0x80000, CRC(683e96bc) SHA1(bca8e87bea9f7044fa29dc4518e2ac5b429e3313) )
	ROM_LOAD32_WORD( "rhg073003.u12", 0x100002, 0x80000, CRC(8313b03b) SHA1(d2a91bae8063d89ec9a1edab6df3e6711719d2c2) )
	ROM_LOAD32_WORD( "rhg073003.u9",  0x200000, 0x80000, CRC(9c08aefa) SHA1(fe3ffa8eb308ab216cc08dd2ce51113b4ef74c4a) )
	ROM_LOAD32_WORD( "rhg073003.u13", 0x200002, 0x80000, CRC(69fd4f89) SHA1(4e0469caecf9293197a4a5de960eb9dcfee39ca3) )
	ROM_LOAD32_WORD( "rhg073003.u10", 0x300000, 0x80000, CRC(9aae49d7) SHA1(5cf87b747ea7561766fe0ffc15967fea657b252b) )
	ROM_LOAD32_WORD( "rhg073003.u14", 0x300002, 0x80000, CRC(240f7759) SHA1(1fa5ba0185b027101dae207ec5d28b07d3d73fc2) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// RHG0730-99 / 596 / 10 Credit Multiplier / 9 Line Multiline
// Chicken / Export / C - 23/02/98
// Marked as RHG0730 99.
// Variation (% and NO): 87.938% 99
// No set chips required
// All devices are 27c4002 instead of 27c4096.
ROM_START( chickna5ua )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0a6917 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x1a4ad4b2
	        Calculated Checksum 0x1a4ad4b2  (OK)
	    0x0a6918-0x35040b is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "rhg073099.u7",  0x000000, 0x80000, CRC(0a006791) SHA1(b1df734e316222590d11c33c220c2f980103b327) )
	ROM_LOAD32_WORD( "rhg073099.u11", 0x000002, 0x80000, CRC(5b338809) SHA1(82e485504a6d5c3ad1840d4c22ddedbeb948f8f9) )
	ROM_LOAD32_WORD( "rhg073099.u8",  0x100000, 0x80000, CRC(683e96bc) SHA1(bca8e87bea9f7044fa29dc4518e2ac5b429e3313) )
	ROM_LOAD32_WORD( "rhg073099.u12", 0x100002, 0x80000, CRC(8313b03b) SHA1(d2a91bae8063d89ec9a1edab6df3e6711719d2c2) )
	ROM_LOAD32_WORD( "rhg073099.u9",  0x200000, 0x80000, CRC(9c08aefa) SHA1(fe3ffa8eb308ab216cc08dd2ce51113b4ef74c4a) )
	ROM_LOAD32_WORD( "rhg073099.u13", 0x200002, 0x80000, CRC(69fd4f89) SHA1(4e0469caecf9293197a4a5de960eb9dcfee39ca3) )
	ROM_LOAD32_WORD( "rhg073099.u10", 0x300000, 0x80000, CRC(9aae49d7) SHA1(5cf87b747ea7561766fe0ffc15967fea657b252b) )
	ROM_LOAD32_WORD( "rhg073099.u14", 0x300002, 0x80000, CRC(240f7759) SHA1(1fa5ba0185b027101dae207ec5d28b07d3d73fc2) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 01J01886 / 596/1 / 50 Credit Multiplier/9 Line Multiline
// Chicken / Local / B - 23/12/96
// ROM contains unaltered NSW/ACT "Local" region string, but game is for the Venezuelan market
// Game is in Spanish, however audit mode is in English
ROM_START( chickna5v )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b80
	    0x000000-0x07c3e7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x9e99a201
	        Calculated Checksum 0x9e99a201  (OK)
	    0x07c3e8-0x2f5d37 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	// the checksum only covers part of the first 2 roms, marked all as BAD_DUMP because it can't be trusted without a full redump.
	ROM_LOAD32_WORD( "01j01886.u7",  0x000000, 0x80000, BAD_DUMP CRC(6852bf73) SHA1(a53c8467f4a128da26707a1fe239f32330dffe0a) )
	ROM_LOAD32_WORD( "01j01886.u11", 0x000002, 0x80000, BAD_DUMP CRC(c065aaa2) SHA1(875c2e9e432d4532e116dc197613327a06841fc2) )
	ROM_LOAD32_WORD( "01j01886.u8",  0x100000, 0x80000, BAD_DUMP CRC(81ad8cc2) SHA1(006dbc0b352187425bc410903178bad912677a29) )
	ROM_LOAD32_WORD( "01j01886.u12", 0x100002, 0x80000, BAD_DUMP CRC(1f03e878) SHA1(b9beee74f5cfd942ba50f8d2955ff588dea32842) )
	ROM_LOAD32_WORD( "01j01886.u9",  0x200000, 0x80000, BAD_DUMP CRC(2471f934) SHA1(486757b919f4c4af40b707175d38eca26cda1ea3) )
	ROM_LOAD32_WORD( "01j01886.u13", 0x200002, 0x80000, BAD_DUMP CRC(231ca698) SHA1(eb237652974228994e793f0dc87de93095db3c9c) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100919V / 577/7 / 25 Credit Multiplier/20 Line Multiline
// Coral Riches II / Local / A - 29/12/98
ROM_START( coralrc2 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000be8
	    0x000000-0x05ba63 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x12fce303
	        Calculated Checksum 0x12fce303  (OK)
	    0x05ba64-0x12b3e3 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05ba64-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100919v.u7",  0x000000, 0x80000, CRC(02c430c3) SHA1(f4bae1aa5437af1df2a04f700da044bc4fb652b7) )
	ROM_LOAD32_WORD( "0100919v.u11", 0x000002, 0x80000, CRC(8cd17e90) SHA1(c6d6a29e62ca6e1b278a2e1d1b358e10ca2de4ed) )
	ROM_LOAD32_WORD( "0100919v.u8",  0x100000, 0x80000, CRC(1ee9557c) SHA1(3bee295509d4b0c11ce41a7a20ba91230b7cb4ca) )
	ROM_LOAD32_WORD( "0100919v.u12", 0x100002, 0x80000, CRC(9ea140b5) SHA1(11f6b9ab60117f236b464c9dbc939dfb8f240359) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200753V / 615/1 / 10 Credit Multiplier / 20 Line Multiline
// Cuckoo / Local / D - 03/07/98
ROM_START( cuckoo )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b10
	    0x000000-0x05f63f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x6aa5ad46
	        Calculated Checksum 0x6aa5ad46  (OK)
	    0x05f640-0x1b1deb is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05f640-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200753v.u7",  0x000000, 0x80000, CRC(5c7ef84a) SHA1(59563a076ecf391ac1779e0dcd530a1ea158a4e3) )
	ROM_LOAD32_WORD( "0200753v.u11", 0x000002, 0x80000, CRC(a69c1416) SHA1(7fe57a194bf29346c039dfac1326f3ee5080e630) )
	ROM_LOAD32_WORD( "0200753v.u8",  0x100000, 0x80000, CRC(a7b4242c) SHA1(4e6961e9b3267d17b93075c41a691a8033a34d90) )
	ROM_LOAD32_WORD( "0200753v.u12", 0x100002, 0x80000, CRC(cb706eb7) SHA1(cbd6235ca7a29c78ef2cb659d9c21466ed39b360) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// CHG1195 / MV4104 / 3,5,10,20,25,50 Credit Multiplier / 9-20 Line Multiline
// CUCKOO / Export C / 02/02/00
// ROM says '9-20 Line Multiline', but game only has 9 Lines
// All devices are 27c4002 instead of 27c4096
// Requires set chips 4.01.xx
ROM_START( cuckoou )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0a588b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x9e544942
	        Calculated Checksum 0x9e544942  (OK)
	    0x0a588c-0x184b17 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0a588c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "chg1195.u7",  0x000000, 0x80000, CRC(0bd17338) SHA1(b8f467bdf8d76533a2b7d44fe93be414f25a3c31) )
	ROM_LOAD32_WORD( "chg1195.u11", 0x000002, 0x80000, CRC(4c407deb) SHA1(57589e61a376ddff99cd420eb47bf8c902c6a249) )
	ROM_LOAD32_WORD( "chg1195.u8",  0x100000, 0x80000, CRC(33f52052) SHA1(89cbfe588d91244adff4c520fa94962d69ff20bf) )
	ROM_LOAD32_WORD( "chg1195.u12", 0x100002, 0x80000, CRC(00bb7597) SHA1(f4d6b21091e320a82d59477469340633b001ed0d) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )

	ROM_REGION16_BE( 0x100, "eeprom0", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom0", 0x000000, 0x000100, CRC(fea8a821) SHA1(c744cac6af7621524fc3a2b0a9a135a32b33c81b) )

	ROM_REGION16_BE( 0x100, "eeprom1", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom1", 0x000000, 0x000100, CRC(415b9c77) SHA1(86a3b3aabd81f5fcf767dd53f7034f7d58f2020e) )

	ROM_REGION( 0x80000, "nvram", 0 )
	ROM_LOAD( "nvram", 0x000000, 0x080000, CRC(64c895fe) SHA1(12c75338dd1b2260d0581744cef1b705c718727f) )

	ROM_REGION( 0x20, "rtc", 0 )
	ROM_LOAD( "rtc", 0x000000, 0x00001f, CRC(6909acb0) SHA1(6a4589599cd1c477e916474e7b029e9a4e92019b) )
ROM_END


// 0300111V / 577/2 / 20 Credit Multiplier/9 Line Multiline
// Desert Bloom / Local / A - 12/10/95
ROM_START( dstbloom )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000adc
	    0x000000-0x044573 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xe2c025f9
	        Calculated Checksum 0xe2c025f9  (OK)
	    0x044574-0x1cb32b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x044574-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0300111v.u7",  0x000000, 0x80000, CRC(70ba3771) SHA1(d03b23c27a80bab883f18ca3404f7a20989c1dd6) )
	ROM_LOAD32_WORD( "0300111v.u11", 0x000002, 0x80000, CRC(9a656fb9) SHA1(219354ae79e95948963ab618ba2f45f8b614f9dc) )
	ROM_LOAD32_WORD( "0300111v.u8",  0x100000, 0x80000, CRC(5e29eceb) SHA1(4c4b16412aedc521959446585d5aa7e67c19bae5) )
	ROM_LOAD32_WORD( "0300111v.u12", 0x100002, 0x80000, CRC(10cf45b3) SHA1(3f47682ed95f65bdb267f911e113e329ad448167) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200111V / 577/2 / 20 Credit Multiplier/9 Line Multiline
// Desert Bloom / Local / A - 12/10/95
// Same strings as dstbloom but earlier version
ROM_START( dstblooma )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000adc
	    0x000000-0x0431d3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x3a2c9103
	        Calculated Checksum 0x3a2c9103  (OK)
	    0x0431d4-0x1cb32b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0431d4-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200111v.u7",  0x000000, 0x80000, CRC(fbfaa3fe) SHA1(3f915261503fc97eb556422e9ccdac81372c04cc) )
	ROM_LOAD32_WORD( "0200111v.u11", 0x000002, 0x80000, CRC(ed4e8dca) SHA1(1953033e570634cbcf8cd11194c14c57ffc6be53) )
	ROM_LOAD32_WORD( "0200111v.u8",  0x100000, 0x80000, CRC(cc0d567c) SHA1(c4da3d0c0c4420a9f8fbb6403db983b3e27d4b50) )
	ROM_LOAD32_WORD( "0200111v.u12", 0x100002, 0x80000, CRC(0ad41815) SHA1(131efc6ed45d8f44a667bd30380c9e37c64f2c42) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1533 / MV4115_5 / 5, 10, 25, 50 Credit Multiplier / 20 Line Multiline
// Diamond Destiny / Export / A - 09/05/2000
ROM_START( diamdest )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to ROMs being corrupted, all files are missing bytes consisting of 0x0D
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1533.u7",  0x000000, 0x7efb1, BAD_DUMP CRC(b228ed66) SHA1(a92e403b4df2054693787f48e988613843731f9e) )
	ROM_LOAD32_WORD( "ahg1533.u11", 0x000002, 0x7ff0f, BAD_DUMP CRC(a1c66732) SHA1(2b3fa6c86a2f43f3b857c3bff55343859dd52943) )
	ROM_LOAD32_WORD( "ahg1533.u8",  0x100000, 0x7faf6, BAD_DUMP CRC(66b7e33b) SHA1(5e337aeda6f99de16a655bd635ea65f1d2145a67) )
	ROM_LOAD32_WORD( "ahg1533.u12", 0x100002, 0x7faef, BAD_DUMP CRC(615c2343) SHA1(9bb85f97ca8345d80e088fbb7ac5caea360af529) )
	ROM_LOAD32_WORD( "ahg1533.u9",  0x200000, 0x7fff6, BAD_DUMP CRC(be004ff5) SHA1(2827e5be3c21acf2002647729f163659195a461a) )
	ROM_LOAD32_WORD( "ahg1533.u13", 0x200002, 0x7ffeb, BAD_DUMP CRC(612a6bf2) SHA1(01ee8854204da0610aa4ab3a36c3e517906d2ab4) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101018V / 640 / 9 Credit Multiplier/3 Line Multiline
// Diamond Dove / NSW/ACT / B - 19/05/99
// ROM says '9 Credit Multiplier' but game has a 3 credit multiplier
ROM_START( diamdove )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b78
	    0x000000-0x063a9f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x2dfce931
	        Calculated Checksum 0x2dfce931  (OK)
	    0x063aa0-0x273ea3 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x063aa0-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101018v.u7",  0x000000, 0x80000, CRC(2ebb3704) SHA1(42567d873d6ab9221d09e5449fa57b557677d2ab) )
	ROM_LOAD32_WORD( "0101018v.u11", 0x000002, 0x80000, CRC(ff4c684a) SHA1(6598c24a8717b8e624e387f000c584ec3b10a8cd) )
	ROM_LOAD32_WORD( "0101018v.u8",  0x100000, 0x80000, CRC(daa55b3b) SHA1(7aa96a51a3ea9f96c38d08e486eccc54ca4396a3) )
	ROM_LOAD32_WORD( "0101018v.u12", 0x100002, 0x80000, CRC(62209e81) SHA1(68383068de2e030467c3f3ac16459ae2f3b2cce6) )
	ROM_LOAD32_WORD( "0101018v.u9",  0x200000, 0x80000, CRC(2254f0e9) SHA1(5bccd65e7e616e1f6ed08a0c84862cb13f9f7098) )
	ROM_LOAD32_WORD( "0101018v.u13", 0x200002, 0x80000, CRC(952a850f) SHA1(66da391af532f9ef531d10995c96a90eb71cd09a) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200302V / 483/7 / 3 Credit Multiplier/3 Line Multiline
// Diamond Fever / Local / E - 05/09/96
ROM_START( dmdfever )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ad8
	    0x000000-0x054f3f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x87d3b331
	        Calculated Checksum 0x87d3b331  (OK)
	    0x054f40-0x0ef137 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200302v.u7",  0x000000, 0x80000, CRC(d90032f9) SHA1(9c34e626168bdfa3ff2722d9ff1970d826135cf7) )
	ROM_LOAD32_WORD( "0200302v.u11", 0x000002, 0x80000, CRC(29620f05) SHA1(172b6226c443931f0c4ddc44a63c8fc0e6be3824) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASEFF )
ROM_END


// 0400433V / 604 / 10 Credit Multiplier/9 Line Multiline
// Diamond Touch / Local / E - 30/06/97
// Touchscreen game
ROM_START( dimtouch )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found (uses different startup sequence)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0400433v.u7",  0x000000, 0x80000, CRC(71b19365) SHA1(5a8ba1806af544d33e9acbcbbc0555805b4074e6) )
	ROM_LOAD32_WORD( "0400433v.u11", 0x000002, 0x80000, CRC(3d836342) SHA1(b015a4ba998b39ed86cdb6247c9c7f1365641b59) )
	ROM_LOAD32_WORD( "0400433v.u8",  0x100000, 0x80000, CRC(971bbf63) SHA1(082f81115209c7089c76fb207248da3c347a080b) )
	ROM_LOAD32_WORD( "0400433v.u12", 0x100002, 0x80000, CRC(9e0d08e2) SHA1(38b10f7c37f1cefe9271549073dc0a4fed409aec) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASEFF )
ROM_END


// 0200424V / 602/1 / 10 Credit Multiplier / 20 Line Multiline
// Dolphin Treasure / NSW/ACT / B - 06/12/96
ROM_START( dolphntr )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b08
	    0x000000-0x05c367 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x14ccd8a1
	        Calculated Checksum 0x14ccd8a1  (OK)
	    0x05c368-0x0fe787 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200424v.u7",  0x000000, 0x80000, CRC(5dd88306) SHA1(ee8ec7d123d057e8df9be0e8dadecea7dab7aafd) )
	ROM_LOAD32_WORD( "0200424v.u11", 0x000002, 0x80000, CRC(bcb732ea) SHA1(838300914846c6e740780e5a24b9db7304a8a88d) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100424V / 602/1 / 10 Credit Multiplier / 20 Line Multiline
// Dolphin Treasure / NSW/ACT / B - 06/12/96
// Same strings as dolphntr but earlier version
ROM_START( dolphntra )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b08
	    0x000000-0x053897 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x81967fa4
	        Calculated Checksum 0x81967fa4  (OK)
	    0x053898-0x1cac2f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x053898-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100424v.u7",  0x000000, 0x80000, CRC(657faef7) SHA1(09e1f9d461e855c10cf8b825ef83dd3e7db65b43) )
	ROM_LOAD32_WORD( "0100424v.u11", 0x000002, 0x80000, CRC(65aa46ec) SHA1(3ad4270efbc2e947097d94a3258a544d79a1d599) )
	ROM_LOAD32_WORD( "0100424v.u8",  0x100000, 0x80000, CRC(e77868ad) SHA1(3345da120075bc0da47bac0a4840790693382620) )
	ROM_LOAD32_WORD( "0100424v.u12", 0x100002, 0x80000, CRC(6abd9309) SHA1(c405a13f5bfe447c1ab20d92e140e4fb145920d4) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100388V / 602 / 10 Credit Multiplier / 9 Line Multiline
// Dolphin Treasure / NSW/ACT / B - 10/12/96
ROM_START( dolphntrb )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b20
	    0x000000-0x0536c3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xeee6e6fc
	        Calculated Checksum 0xeee6e6fc  (OK)
	    0x0536c4-0x1ce293 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0536c4-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100388v.u7",  0x000000, 0x80000, CRC(7463b5f6) SHA1(89e5cf8143d0b4ed54aa2c9bd8840f0aba19322e) )
	ROM_LOAD32_WORD( "0100388v.u11", 0x000002, 0x80000, CRC(8e391b67) SHA1(4b7a7295d3a96e26bf1958eb30af0b6582a5e5a6) )
	ROM_LOAD32_WORD( "0100388v.u8",  0x100000, 0x80000, CRC(195bec0f) SHA1(86bdc53e682476c2d90c5e51d4bccdc048d22e7f) )
	ROM_LOAD32_WORD( "0100388v.u12", 0x100002, 0x80000, CRC(1a1fbbcf) SHA1(6e3772dcccd9b5958bec3bfac9af22b2eabca32e) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1607 / MV4090 / 9 & 20 Line Multiline / 3, 5, 10, 20, 25, 50 Credit Multiplier
// Dolphin Treasure / Export / D - 22/12/99
// Cash Express Hyperlink game, but can also run standalone without progressive jackpot
// ROM says '9 & 20 Line Multiline' but game only has 20 lines with a 5, 10, 25 or 50 credit multiplier
// Requires set chips 4.04.xx
ROM_START( dolphntrce )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0f24a3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x06f7ea7e
	        Calculated Checksum 0x06f7ea7e  (OK)
	    0x0f24a4-0x356213 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1607.u7",  0x000000, 0x80000, CRC(60a4643f) SHA1(e72de7218ee57d5fc0b316252366437592ef6000) )
	ROM_LOAD32_WORD( "ahg1607.u11", 0x000002, 0x80000, CRC(55d65ff6) SHA1(e36dd58fbaf1fb5fbcc6586535acff6af5f23067) )
	ROM_LOAD32_WORD( "ahg1607.u8",  0x100000, 0x80000, CRC(b2b403e7) SHA1(00ea248773a2acc4c5d71a24ce22f206df1888b2) )
	ROM_LOAD32_WORD( "ahg1607.u12", 0x100002, 0x80000, CRC(f3bc56c5) SHA1(d48780ba6c261439600cb4b07bb9b8d0143993b2) )
	ROM_LOAD32_WORD( "ahg1607.u9",  0x200000, 0x80000, CRC(3ada71cd) SHA1(74471ab845f8ceda6a74673be70547a8b49baddc) )
	ROM_LOAD32_WORD( "ahg1607.u13", 0x200002, 0x80000, CRC(cb057b1e) SHA1(7853305fa618bfd34b418cd1c3519b3bb8a7d8f0) )
	ROM_LOAD32_WORD( "ahg1607.u10", 0x300000, 0x80000, CRC(84d056b3) SHA1(eb3c496fae1e35cc334ff3bb92d444d9fd00efee) )
	ROM_LOAD32_WORD( "ahg1607.u14", 0x300002, 0x80000, CRC(6f522ffb) SHA1(0fbba6b8df15631e4361daf505469f2214ad8695) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1606 / MV4090 / 9 & 20 Line Multiline / 3, 5, 10, 20, 25, 50 Credit Multiplier
// Dolphin Treasure / Export / D - 22/12/99
// Cash Express Hyperlink game, but can also run standalone without progressive jackpot
// ROM says '9 & 20 Line Multiline' but game only has 9 lines with a 5, 10 or 20 credit multiplier
// Requires set chips 4.04.xx
ROM_START( dolphntrcea )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0f2307 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x60cc71fc
	        Calculated Checksum 0x60cc71fc  (OK)
	    0x0f2308-0x356077 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1606.u7",  0x000000, 0x80000, CRC(d468edf7) SHA1(100672d09184e06130ce253749bd9e20ee0a06d4) )
	ROM_LOAD32_WORD( "ahg1606.u11", 0x000002, 0x80000, CRC(0fe64635) SHA1(b504216e59984951b46701019f87cad759ab60f2) )
	ROM_LOAD32_WORD( "ahg1606.u8",  0x100000, 0x80000, CRC(a53a2de4) SHA1(1741af795f88e867021f3c08d8990611d893a8e8) )
	ROM_LOAD32_WORD( "ahg1606.u12", 0x100002, 0x80000, CRC(c2e268a2) SHA1(6aeb27ae844dbf495c64be210bcac97f4c7a6969) )
	ROM_LOAD32_WORD( "ahg1606.u9",  0x200000, 0x80000, CRC(fe9146f7) SHA1(95bef8910d213d588d45ec2639828a31aab6603c) )
	ROM_LOAD32_WORD( "ahg1606.u13", 0x200002, 0x80000, CRC(d6aa89fe) SHA1(eccb49d49f533aeed9fefb14018bcc06d3fdaf23) )
	ROM_LOAD32_WORD( "ahg1606.u10", 0x300000, 0x80000, CRC(0be76189) SHA1(a458f620f48b9f4a73f59d31ba98864c5a64e1d7) )
	ROM_LOAD32_WORD( "ahg1606.u14", 0x300002, 0x80000, CRC(c6c59ed6) SHA1(0ce8e5824c5937ffe2eeb34320db9dc568bca7cb) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1519 / MV4090 / 9 & 20 Line Multiline / 3, 5, 10, 20, 25, 50 Credit Multiplier
// Dolphin Treasure / Export / D - 22/12/99
// Cash Express Hyperlink game, but can also run standalone without progressive jackpot
// ROM says '9 & 20 Line Multiline' but game only has 20 lines
// Requires set chips 4.04.xx
ROM_START( dolphntrceb )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0eeb03 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x18884f4e
	        Calculated Checksum 0x18884f4e  (OK)
	    0x0eeb04-0x35287b is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1519.u7",  0x000000, 0x80000, CRC(7246836c) SHA1(95e6230bf49bb9099b497ee6ff11cd69279ecc6d) )
	ROM_LOAD32_WORD( "ahg1519.u11", 0x000002, 0x80000, CRC(e7ea2c1f) SHA1(85f462aa5fdc9528081e81151d8fad2fe9fbda3d) )
	ROM_LOAD32_WORD( "ahg1519.u8",  0x100000, 0x80000, CRC(0110edaf) SHA1(84367fd01daff36c25aff591ab3eecfc841b4d19) )
	ROM_LOAD32_WORD( "ahg1519.u12", 0x100002, 0x80000, CRC(209953f4) SHA1(3517f871ab635bb9497cfa45cc61051f38189fd0) )
	ROM_LOAD32_WORD( "ahg1519.u9",  0x200000, 0x80000, CRC(95539a1f) SHA1(5a8898a3c3b3970453f7048a81bbcfa2c1b34be1) )
	ROM_LOAD32_WORD( "ahg1519.u13", 0x200002, 0x80000, CRC(82e7be90) SHA1(b513a75eb6514f10493534e46f69ed7f5a470cd3) )
	ROM_LOAD32_WORD( "ahg1519.u10", 0x300000, 0x80000, CRC(bf226a58) SHA1(2d726c7b53652f0782a942aeaa15295454378ce3) )
	ROM_LOAD32_WORD( "ahg1519.u14", 0x300002, 0x80000, CRC(f2da081c) SHA1(5202741719d72a9290bda2c646d37e8c8ad41c04) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101250V / 602/17 / 25 Credit Multiplier / 20 Line Multiline
// Dolphin Treasure / QCOM / A - 20/01/00
ROM_START( dolphntrql )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ad0
	    0x000000-0x0806ab is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xb0e71f34
	        Calculated Checksum 0xb0e71f34  (OK)
	    0x0806ac-0x1ad0c7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0806ac-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101250v.u7",  0x000000, 0x80000, CRC(4d38af9a) SHA1(4f0bc990b33b8421bde4473d22ad8ca55fc407c9) )
	ROM_LOAD32_WORD( "0101250v.u11", 0x000002, 0x80000, CRC(f9750215) SHA1(8e44be5c83a1f75a95c856729cfc84c270649caf) )
	ROM_LOAD32_WORD( "0101250v.u8",  0x100000, 0x80000, CRC(7d82a406) SHA1(c3757597fe9c484de71d81cb02046f6cca27ff44) )
	ROM_LOAD32_WORD( "0101250v.u12", 0x100002, 0x80000, CRC(35236e6f) SHA1(f196f12a1761a235ad66f4e181ca249b1df7e245) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// FHG4077-02 / 602/1 / 10 Credit Multiplier / 9 Line Multiline
// Dolphin Treasure / Export / B - 06/12/96
// All devices are 27c4002 instead of 27c4096
// No set chips required
ROM_START( dolphntru )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x08ec8b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x9caf255e
	        Calculated Checksum 0x9caf255e  (OK)
	    0x08ec8c-0x13d99f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x08ec8c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "fhg407702.u7",  0x000000, 0x80000, CRC(97e3e4d0) SHA1(211b9b9e0f25dfaf9d1dfe1d3d88592522aa6f07) )
	ROM_LOAD32_WORD( "fhg407702.u11", 0x000002, 0x80000, CRC(de221eb5) SHA1(0e550e90b7fd5670f3f3a8589239c342ed70dc3d) )
	ROM_LOAD32_WORD( "fhg407702.u8",  0x100000, 0x80000, CRC(cb3ca8b6) SHA1(dba8bdaa406c07870f95241466359e39a012a70b) )
	ROM_LOAD32_WORD( "fhg407702.u12", 0x100002, 0x80000, CRC(8ee1c2d3) SHA1(e6ecaaac0cb4518ecc0d36532ab532f46e3e628b) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )

	ROM_REGION16_BE( 0x100, "eeprom0", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom0", 0x000000, 0x000100, CRC(fea8a821) SHA1(c744cac6af7621524fc3a2b0a9a135a32b33c81b) )

	ROM_REGION16_BE( 0x100, "eeprom1", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom1", 0x000000, 0x000100, CRC(1fc27753) SHA1(7e5008faaf115dc506481430272285117c989d8e) )

	ROM_REGION( 0x80000, "nvram", 0 )
	ROM_LOAD( "nvram", 0x000000, 0x080000, CRC(0063e5ca) SHA1(a3d7b636bc9d792e93d11cb2babf24fbdd6d7776) )

	ROM_REGION( 0x20, "rtc", 0 )
	ROM_LOAD( "rtc", 0x000000, 0x00001f, CRC(6909acb0) SHA1(6a4589599cd1c477e916474e7b029e9a4e92019b) )
ROM_END


// 0100521V / 610 / 10 Credit Multiplier/10 Line Multiline
// Dragon's Eye / Local / A - 09/05/97
// ROM says '10 Line Multiline' but game only has 9 lines
ROM_START( drgneye )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000af8
	    0x000000-0x05891b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x2b2fe66e
	        Calculated Checksum 0x2b2fe66e  (OK)
	    0x05891c-0x0e689f is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100521v.u7",  0x000000, 0x80000, CRC(db9c952d) SHA1(4cbe3ffe6cf0bb112cb9a2d7a4ff0b28154d32c1) )
	ROM_LOAD32_WORD( "0100521v.u11", 0x000002, 0x80000, CRC(2bb47749) SHA1(796f610e5202b5eb26a6e901d43ee5d9e3f95332) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200586V / 606/2 / 20 Credit Multiplier/9 Line Multiline
// Dream Weaver / Local / A- 20/06/97
// Touchscreen game
ROM_START( dreamwv )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found (uses different startup sequence)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200586v.u7",  0x000000, 0x80000, CRC(6d52fcd1) SHA1(136cb89037a96bf6824ed5754fc67167f0287684) )
	ROM_LOAD32_WORD( "0200586v.u11", 0x000002, 0x80000, CRC(6b0d58b8) SHA1(3c70d294673deb38d737099880fdbd04e2dc20e6) )
	ROM_LOAD32_WORD( "0200586v.u8",  0x100000, 0x80000, CRC(d0a2fb07) SHA1(a05468f36ee1024399780e92825803908f416d80) )
	ROM_LOAD32_WORD( "0200586v.u12", 0x100002, 0x80000, CRC(0b3e03d3) SHA1(966ec84aff686ad360d440995b81ae469539a5b5) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 01J00081 / JB004 / Multi credit / Multi line
// Dynamite Jack / NSW/ACT / A - 12/07/2000
ROM_START( dynajack )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b78
	    0x000000-0x07031b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xd8815d1c
	        Calculated Checksum 0xd8815d1c  (OK)
	    0x07031c-0x227a4b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x07031c-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "01j00081.u7",  0x000000, 0x80000, CRC(73783ecf) SHA1(280b4da540b405959f31c2eebbf87ab635d21c06) )
	ROM_LOAD32_WORD( "01j00081.u11", 0x000002, 0x80000, CRC(5a0147ae) SHA1(f2135b2525eb50a03a8f6360e7edb92bf0b88740) )
	ROM_LOAD32_WORD( "01j00081.u8",  0x100000, 0x80000, CRC(e686eab2) SHA1(6eb18adda82357ff84f77e9334733094430dfdc6) )
	ROM_LOAD32_WORD( "01j00081.u12", 0x100002, 0x80000, CRC(beee94ff) SHA1(fad0d3506d10330840d3e5fcdfd7f0aa20041969) )
	ROM_LOAD32_WORD( "01j00081.u9",  0x200000, 0x80000, CRC(28a45170) SHA1(d7bb8e4dd24e3a3acf44e7fc40e49ebee5c15ec9) )
	ROM_LOAD32_WORD( "01j00081.u13", 0x200002, 0x80000, CRC(d204ff9c) SHA1(8ac5533928fb3ca247dc85cea67da45a6743f732) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// CHG1562 / US002 / 5,10,20, Credit Multiplier / 9 Line Multiline
// Dynamite Jack / Export / A - 11/07/01
ROM_START( dynajacku )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to ROMs being corrupted, first 4 files are missing bytes consisting of 0x0D
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "chg1562.u7",  0x000000, 0x7f023, BAD_DUMP CRC(c69c989c) SHA1(6eeadf185a38944c6c0c32777c006f27505eaa73) )
	ROM_LOAD32_WORD( "chg1562.u11", 0x000002, 0x7ff1e, BAD_DUMP CRC(693b28cf) SHA1(06ca74d5f7a1e0cd315f4b0dc5182db1f3cfa5a7) )
	ROM_LOAD32_WORD( "chg1562.u8",  0x100000, 0x7fe83, BAD_DUMP CRC(5b585157) SHA1(e7d11b959f02f4cf35413ea59220c751653cb0c7) )
	ROM_LOAD32_WORD( "chg1562.u12", 0x100002, 0x7fe8c, BAD_DUMP CRC(39e6017d) SHA1(5f5dd611b51ad91b6de8bce7de5efc4e48fd02a3) )
	ROM_LOAD32_WORD( "chg1562.u9",  0x200000, 0x80000, CRC(5311546c) SHA1(83eff3a0382a19b315be4612bcdc5280049b10f1) )
	ROM_LOAD32_WORD( "chg1562.u13", 0x200002, 0x80000, CRC(5a2220d7) SHA1(aca5fefb60af93ba776cc695e9a7ea406f527937) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100652V / 623 / 8 Credit Multiplier / 25 Credit Multiway
// El Dorado / Local / B - 24/03/98
ROM_START( eldorda5 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b88
	    0x000000-0x06328b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xed424efa
	        Calculated Checksum 0xed424efa  (OK)
	    0x06328c-0x0d4b57 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100652v.u7",  0x000000, 0x80000, CRC(d9afe87c) SHA1(577ea5da9c4e93a393711a0c7361365301f4241e) )
	ROM_LOAD32_WORD( "0100652v.u11", 0x000002, 0x80000, CRC(35233cf8) SHA1(e02477526f2f9e2663c1876f543d138b2caf28df) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0400122V / 570/3 / 10 Credit Multiplier/9 Line Multiline
// Enchanted Forest / Local / E - 23/06/95
ROM_START( eforsta5 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ae4
	    0x000000-0x045da3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x2c99855f
	        Calculated Checksum 0x2c99855f  (OK)
	    0x045da4-0x0ebd43 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0400122v.u7",  0x000000, 0x80000, CRC(b5829b27) SHA1(f6f84c8dc524dcee95e37b93ead9090903bdca4f) )
	ROM_LOAD32_WORD( "0400122v.u11", 0x000002, 0x80000, CRC(7a97adc8) SHA1(b52f7fdc7edf9ad92351154c01b8003c0576ed94) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// CHG1536 / MV4108/6 / 3,5,10,20,25,50 Credit Multiplier/9 Line Multiline
// Enchanted Forest / Export / C - 17/01/00
// Cash Express Hyperlink game, but can also run standalone without progressive jackpot
ROM_START( eforsta5ce )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to ROMs being corrupted, first 6 files are missing bytes consisting of 0x0D
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "chg1536.u7",  0x000000, 0x7efd4, BAD_DUMP CRC(d29185cc) SHA1(26154f3d99907461cff4a44fe02929fae66e6963) )
	ROM_LOAD32_WORD( "chg1536.u11", 0x000002, 0x7feab, BAD_DUMP CRC(4ea1bd5d) SHA1(86ffabb11550a932006549496772bdd0d27aa384) )
	ROM_LOAD32_WORD( "chg1536.u8",  0x100000, 0x7f753, BAD_DUMP CRC(d439857a) SHA1(8d8d85f36253c89a8e5fb825761284ddd44890c4) )
	ROM_LOAD32_WORD( "chg1536.u12", 0x100002, 0x7f7f9, BAD_DUMP CRC(5f339d63) SHA1(f83587f674e4e12dff65d5c4828e62c4e8349baa) )
	ROM_LOAD32_WORD( "chg1536.u9",  0x200000, 0x7eaaa, BAD_DUMP CRC(5e739d2c) SHA1(2bfae3b39fdb9f52a539aa4532109b51e88ac5c4) )
	ROM_LOAD32_WORD( "chg1536.u13", 0x200002, 0x7eab3, BAD_DUMP CRC(653240e4) SHA1(20a196a2b77416d1490f3d7d4d66dd69ef8c59b2) )
	ROM_LOAD32_WORD( "chg1536.u10", 0x300000, 0x80000, CRC(e1301711) SHA1(b7778b9d3faba0e807b7806f2837d57b0c6a3338) )
	ROM_LOAD32_WORD( "chg1536.u14", 0x300002, 0x80000, CRC(113238a6) SHA1(145467e1f015543d23bb4a377d71949693f21c34) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1615 / MV4108/6 / 3,5,10,20,25,50 Credit Multiplier / 9 Line Multiline
// Enchanted Forest / Export / C - 17/01/00
// Marked as SPC.
// All devices are 27C4096.
// Cash Express Hyperlink game, but can also run standalone without progressive jackpot
// Requires set chips 4.04.xx
ROM_START( eforsta5cea )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0f29e7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xdb27a86b
	        Calculated Checksum 0xdb27a86b  (OK)
	    0x0f29e8-0x31ee8f is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1615.u7",  0x000000, 0x80000, CRC(a08e6be6) SHA1(ab7aa0a333fbf1c4bbea82a55651b2c32b0c9fb8) )
	ROM_LOAD32_WORD( "ahg1615.u11", 0x000002, 0x80000, CRC(4cbbeb31) SHA1(76d6283627be2e4e7e9e097aa9e4590348a018e7) )
	ROM_LOAD32_WORD( "ahg1615.u8",  0x100000, 0x80000, CRC(cf79a2b7) SHA1(fb2c5ef26c81fc5d8f572f1e97d2cbadfd9a6f70) )
	ROM_LOAD32_WORD( "ahg1615.u12", 0x100002, 0x80000, CRC(2eb4b2bf) SHA1(c1324aadb00980aca1edcc1d3dae8f6b6e76991b) )
	ROM_LOAD32_WORD( "ahg1615.u9",  0x200000, 0x80000, CRC(04b8e743) SHA1(11aee450b82906067b99fd3fa11cd1842d95851f) )
	ROM_LOAD32_WORD( "ahg1615.u13", 0x200002, 0x80000, CRC(68a732f0) SHA1(f98ba7f1d6def1783e819291ff8a58f6a7f72350) )
	ROM_LOAD32_WORD( "ahg1615.u10", 0x300000, 0x80000, CRC(cb131b54) SHA1(8075be3f0251e696254336277703f9b956097a50) )
	ROM_LOAD32_WORD( "ahg1615.u14", 0x300002, 0x80000, CRC(961fd1b3) SHA1(59c2280258b98bc0153b6dc08d1af026de5b3ed3) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// JHG0415-03 / MV4033 / 10 Credit Multiplier / 9 Line Multiline
// Enchanted Forest / Export / B - 10/02/97
// Marked as 94.97%
// All devices are 27c4002 instead of 27c4096.
// No set chips required
ROM_START( eforsta5u )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0a5233 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x5de71535
	        Calculated Checksum 0x5de71535  (OK)
	    0x0a5234-0x15dbdf is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0a5234-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "jhg041503.u7",  0x000000, 0x80000, CRC(48a87915) SHA1(103f79f482b4e1ba3653280cdcf1ac6718981faf) )  // 94.97%
	ROM_LOAD32_WORD( "jhg041503.u11", 0x000002, 0x80000, CRC(beb31b6d) SHA1(454fe5b04789e6f45d82d92f808fde5770435a80) )  // 94.97%
	ROM_LOAD32_WORD( "jhg041503.u8",  0x100000, 0x80000, CRC(002dec6c) SHA1(fb3f4ce9cd8cd9e0e3133376ed014db83db041c5) )  // base
	ROM_LOAD32_WORD( "jhg041503.u12", 0x100002, 0x80000, CRC(c968471f) SHA1(9d54a5c396e6f83690db2fcb7ddcc8a47a7dd777) )  // base

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// JHG0415-99 / MV4033 / 10 Credit Multiplier / 9 Line Multiline
// Enchanted Forest / Export / B - 10/02/97
// Variation (% and NO): 88.26% 99
// All devices are 27c4002 instead of 27c4096.
// No set chips required
ROM_START( eforsta5ua )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0a5233 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x5fed2247
	        Calculated Checksum 0x5fed2247  (OK)
	    0x0a5234-0x15dbdf is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0a5234-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "jhg041599.u7",  0x000000, 0x80000, CRC(394d93a0) SHA1(5dd91ef55da4b6c8f0866c21d8d4ae9e18ab1bb0) )  // 88,26%
	ROM_LOAD32_WORD( "jhg041599.u11", 0x000002, 0x80000, CRC(3f542a64) SHA1(36de7fc85f2424a62f322de2ef82e061d2335526) )  // 88,26%
	ROM_LOAD32_WORD( "jhg041599.u8",  0x100000, 0x80000, CRC(002dec6c) SHA1(fb3f4ce9cd8cd9e0e3133376ed014db83db041c5) )  // base
	ROM_LOAD32_WORD( "jhg041599.u12", 0x100002, 0x80000, CRC(c968471f) SHA1(9d54a5c396e6f83690db2fcb7ddcc8a47a7dd777) )  // base

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100651V / 624 / 3 Credit Multiplier / 3 Line Multiline
// Fast Fortune / Local / D - 07/05/98
ROM_START( fastfort )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ba8
	    0x000000-0x05c0e7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x4af4f2eb
	        Calculated Checksum 0x4af4f2eb  (OK)
	    0x05c0e8-0x1e5b5b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05c0e8-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100651v.u7",  0x000000, 0x80000, CRC(a68d21ff) SHA1(082d2985d9037465d998d9176b7e5447189fae01) )
	ROM_LOAD32_WORD( "0100651v.u11", 0x000002, 0x80000, CRC(2945baed) SHA1(bcafb84e3935912e47b4396c488ecfd3c1b19124) )
	ROM_LOAD32_WORD( "0100651v.u8",  0x100000, 0x80000, CRC(fbad0352) SHA1(d202d46f117095ac19347b9cd31e7252b5f76d6e) )
	ROM_LOAD32_WORD( "0100651v.u12", 0x100002, 0x80000, CRC(d591dfb6) SHA1(9a1c1070b7e8774928d684c45481d72ce5108bf1) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 01J00131 / JB006 / Multi credit / Multi line
// Fortune Teller / NSW/ACT / D - 24/11/2000
ROM_START( fortellr )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b78
	    0x000000-0x07038b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x49e7e64e
	        Calculated Checksum 0x49e7e64e  (OK)
	    0x07038c-0x3616a7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x07038c-0x3fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "01j00131.u7",  0x000000, 0x80000, CRC(78394106) SHA1(aedfb98d7aa515eebabf378edb9c43e01bcba010) )
	ROM_LOAD32_WORD( "01j00131.u11", 0x000002, 0x80000, CRC(faab1283) SHA1(6200fc2047c4052e4fc3c2d28b26cd9ff67a08be) )
	ROM_LOAD32_WORD( "01j00131.u8",  0x100000, 0x80000, CRC(7ce4ba38) SHA1(43b57e4dc96851f58d95e4f1b99d08f559e27f6a) )
	ROM_LOAD32_WORD( "01j00131.u12", 0x100002, 0x80000, CRC(fe5af3ac) SHA1(f08fe353c871ac4375f0fa25bf15f2638b33a370) )
	ROM_LOAD32_WORD( "01j00131.u9",  0x200000, 0x80000, CRC(a43cd994) SHA1(759fecc809ca1b038d782b173d5638d9be165f9a) )
	ROM_LOAD32_WORD( "01j00131.u13", 0x200002, 0x80000, CRC(d0dd6627) SHA1(ea855da1759a27936615400993b381609071d66c) )
	ROM_LOAD32_WORD( "01j00131.u10", 0x300000, 0x80000, CRC(f2790419) SHA1(8720c37cc678e7c5666c67b9998fbb460a8aad90) )
	ROM_LOAD32_WORD( "01j00131.u14", 0x300002, 0x80000, CRC(507bbe10) SHA1(01b1982c02a00b60aa39ee1b408d653365f728d4) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// BHG1566 / MV4122/2 / 9 Line Multiline / 5, 10, 20 Credit Multiplier
// Fortune Fever / Export / A - 13/05/01
ROM_START( fortfvr )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to ROMs being corrupted, 7 out of 8 files are missing bytes consisting of 0x0D
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "bhg1566.u7",  0x000000, 0x7f050, BAD_DUMP CRC(07c896ae) SHA1(5d275f3759253d2aa3eeef4d6ce973e9a3b5e421) )
	ROM_LOAD32_WORD( "bhg1566.u11", 0x000002, 0x7ff76, BAD_DUMP CRC(e6459275) SHA1(7329204db5d8b9f378918936e46d1b61e6cd2191) )
	ROM_LOAD32_WORD( "bhg1566.u8",  0x100000, 0x7e211, BAD_DUMP CRC(6f26925f) SHA1(8656fe440a123a2fb5e1a801295163bd1244a0c6) )
	ROM_LOAD32_WORD( "bhg1566.u12", 0x100002, 0x7de28, BAD_DUMP CRC(1748216c) SHA1(d2f037b45acbf7d7737d49bf1e6e1c0369d3f7dc) )
	ROM_LOAD32_WORD( "bhg1566.u9",  0x200000, 0x7faf3, BAD_DUMP CRC(7556fbdb) SHA1(95f3f4144a247b9d810a5fd3f219d84147b78bec) )
	ROM_LOAD32_WORD( "bhg1566.u13", 0x200002, 0x7fb29, BAD_DUMP CRC(ed5d7c4a) SHA1(f8288581364bca9ceb96ab9359803c90c520b3ab) )
	ROM_LOAD32_WORD( "bhg1566.u10", 0x300000, 0x80000, CRC(627109ba) SHA1(6689e8cfa7d31fa9471bbde75a5ea97f16ddc00a) )
	ROM_LOAD32_WORD( "bhg1566.u14", 0x300002, 0x7ffff, BAD_DUMP CRC(4fba6570) SHA1(46bb22ba10dc69c70241dfbb00e86ffa5b28fd1c) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// EHG0916-02 / MV4084/1 / 10 Credit Multiplier / 9 Line Multiline
// THE GAMBLER / Export / A - 30/10/98
// Marked as EHG0916 and 92.268%
// All devices are 27c4002 instead of 27c4096
// No set chips required
ROM_START( gambler )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x08f46b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x9eb3c0ef
	        Calculated Checksum 0x9eb3c0ef  (OK)
	    0x08f46c-0x1354cb is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x08f46c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ehg091602.u7",  0x000000, 0x80000, CRC(084167e3) SHA1(ec10a3f1a52ec4a2e2b4e1f303c1cbc048d1331b) )  // 92.268%
	ROM_LOAD32_WORD( "ehg091602.u11", 0x000002, 0x80000, CRC(049b027c) SHA1(8212708c63003e6fd95ce2ee32d46dc4a26ab3c3) )  // 92.268%
	ROM_LOAD32_WORD( "ehg091602.u8",  0x100000, 0x80000, CRC(e2221fdf) SHA1(8a7b2d5de68ae66fe1915a6faac6277249e3fb53) )  // base
	ROM_LOAD32_WORD( "ehg091602.u12", 0x100002, 0x80000, CRC(ebe957f9) SHA1(539945ec9beafe2c83051208370588fce2334f16) )  // base

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101408V / MV4127 / 12 Credit Multiplier/20 Line Multiline
// Geisha / New Zealand / A- 05/03/01
// This game is downported from the MK6 version for the New Zealand market only, no other MK5 version exists
ROM_START( geisha )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found (uses different startup sequence)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101408v.u7",  0x000000, 0x80000, CRC(ebdde248) SHA1(83f4f4deb5c6f5b33ae066d50e043a24cb0cbfe0) )
	ROM_LOAD32_WORD( "0101408v.u11", 0x000002, 0x80000, CRC(2f9e7cd4) SHA1(e9498879c9ca66740856c00fda0416f5d9f7c823) )
	ROM_LOAD32_WORD( "0101408v.u8",  0x100000, 0x80000, CRC(87e41b1b) SHA1(029687aeaed701e0f4b8da9d1d60a5a0a9445518) )
	ROM_LOAD32_WORD( "0101408v.u12", 0x100002, 0x80000, CRC(255f2368) SHA1(eb955452e1ed8d9d4f30f3372d7321f01d3654d3) )
	ROM_LOAD32_WORD( "0101408v.u9",  0x200000, 0x80000, CRC(5f161953) SHA1(d07353d006811813b94cb022857f49c4906fd87b) )
	ROM_LOAD32_WORD( "0101408v.u13", 0x200002, 0x80000, CRC(5ef6323e) SHA1(82a720d814ca06c6d286c59bbf325d9a1034375a) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200894V / 632/1 / 25 Credit Multiplier/20 Line Multiline
// Genie Magic / Local / C- 15/02/99
// Touchscreen game
ROM_START( genmagi )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found (uses different startup sequence)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200894v.u7",  0x000000, 0x80000, CRC(20ec3b50) SHA1(400ad7f86077184fee63690060fe2a51ba888e1b) )
	ROM_LOAD32_WORD( "0200894v.u11", 0x000002, 0x80000, CRC(88c304a3) SHA1(013d5d1d62b356ce5cdf0c9b036c4ca09f191668) )
	ROM_LOAD32_WORD( "0200894v.u8",  0x100000, 0x80000, CRC(341bac7b) SHA1(67df39b8070f6d9afd183b04239d9e2844d588c5) )
	ROM_LOAD32_WORD( "0200894v.u12", 0x100002, 0x80000, CRC(44adc422) SHA1(81256ddebb29fbd69cab8e642faac39635dd1739) )
	ROM_LOAD32_WORD( "0200894v.u9",  0x200000, 0x80000, CRC(ce051dbd) SHA1(433717c5689dc865c1e42669a50e138eae017362) )
	ROM_LOAD32_WORD( "0200894v.u13", 0x200002, 0x80000, CRC(26f51647) SHA1(e980c021d8e2d295ba2d50446b36b85f42d3f318) )
	ROM_LOAD32_WORD( "0200894v.u10", 0x300000, 0x80000, CRC(ea460e72) SHA1(4546e04cc04239528c93e22532db08fccebda8a8) )
	ROM_LOAD32_WORD( "0200894v.u14", 0x300002, 0x80000, CRC(52092ffb) SHA1(6ed591a510e9186588470ec745caf8001712012e) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1623 / MV4112/2 / 5,10,25,50 Credit Multiplier / 20 Line Multiline
// Green Lizard [Reel Game] / Export / A - 05/01/01
// Requires set chip version: 4.04.xx
// Cash Express Hyperlink game, but can also run standalone without progressive jackpot
ROM_START( glizrdce )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0f19bf is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x36907c47
	        Calculated Checksum 0x36907c47  (OK)
	    0x0f19c0-0x3b1443 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1623.u7",  0x000000, 0x80000, CRC(bee526f8) SHA1(32227a6655393a966af3ab698388146f0b752c67) )  // game
	ROM_LOAD32_WORD( "ahg1623.u11", 0x000002, 0x80000, CRC(177ffc12) SHA1(4cedadcf8ddfb95aff7aa0ece96411646677ff96) )  // game
	ROM_LOAD32_WORD( "ahg1623.u8",  0x100000, 0x80000, CRC(22657930) SHA1(8a6db02ce1c45a19d6e6f222788f607224b8af43) )  // base
	ROM_LOAD32_WORD( "ahg1623.u12", 0x100002, 0x80000, CRC(47153192) SHA1(3a384a98c3251a522928a82b391ae88986b6a078) )  // base
	ROM_LOAD32_WORD( "ahg1623.u9",  0x200000, 0x80000, CRC(b4e33204) SHA1(92c7871872ecfbddde9bf04b7e60802d8598cf36) )  // base
	ROM_LOAD32_WORD( "ahg1623.u13", 0x200002, 0x80000, CRC(5c658f42) SHA1(0f1b2d79c0c15562f14e59bb424a9c62df6bd959) )  // base
	ROM_LOAD32_WORD( "ahg1623.u10", 0x300000, 0x80000, CRC(613cb7d8) SHA1(bfd6606f809f5eb2c81802539b5ab7e2a63046a1) )  // base
	ROM_LOAD32_WORD( "ahg1623.u14", 0x300002, 0x80000, CRC(5ceb871c) SHA1(42b8a087fa39261818dd333ea3d8c1ef269671bf) )  // base

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100767V / 625 / 10 Credit Multiplier/20 Line Multiline
// Gnome Around The World 200cm / NSW/ACT / C - 18/12/98
ROM_START( gnomeatw )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x05ebcb is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xd396114d
	        Calculated Checksum 0xd396114d  (OK)
	    0x05ebcc-0x1bf9db is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05ebcc-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100767v.u7",  0x000000, 0x80000, CRC(a5d3825e) SHA1(4ce7466eff770a2c6c3c5de620a14e05bb9fb406) )
	ROM_LOAD32_WORD( "0100767v.u11", 0x000002, 0x80000, CRC(737d7178) SHA1(df788eea23b15415adc94543476b6ad982c4d79b) )
	ROM_LOAD32_WORD( "0100767v.u8",  0x100000, 0x80000, CRC(fe59ec8b) SHA1(b43778b51a0d695c179fa63ce45a47b9f550fb97) )
	ROM_LOAD32_WORD( "0100767v.u12", 0x100002, 0x80000, CRC(49eb3869) SHA1(d98fe385c667872f26d656a3240f557a70ba924f) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1205-03 / MV4091 / 10 Credit Multiplier / 9 Line Multiline
// QUEEN OF THE NILE / NSW/ACT / B - 13/05/97
// ROM contains unaltered Queen of the Nile NSW/ACT region string and date, but game is for the US market
// Marked as AHG1205-03, Golden Pyramids, and 94.941%
// No set chips required
ROM_START( goldpyr )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x08ec83 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x7c8c2fbf
	        Calculated Checksum 0x7c8c2fbf  (OK)
	    0x08ec84-0x1aca63 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x08ec84-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg120503.u7",  0x000000, 0x80000, CRC(2fbed80c) SHA1(fb0d97cb2be96da37c487fc3aef06c6120efdb46) )
	ROM_LOAD32_WORD( "ahg120503.u11", 0x000002, 0x80000, CRC(ec9c183c) SHA1(e405082ee779c4fee103fb7384469c9d6afbc95b) )
	ROM_LOAD32_WORD( "ahg120503.u8",  0x100000, 0x80000, CRC(3cd7d8e5) SHA1(ae83a7c335564c398330d43295997b8ca547c92d) )
	ROM_LOAD32_WORD( "ahg120503.u12", 0x100002, 0x80000, CRC(8bbf45d0) SHA1(f58f28e7cc4ac225197959566d81973b5aa0e836) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1206-99 / 602/2 - 10 Credit Multiplier / 20 Line Multiline
// QUEEN OF THE NILE - NSW/ACT - B - 13/05/97
// ROM contains unaltered Queen of the Nile NSW/ACT region string and date, but game is for the US market
// Marked as AHG1206-99, Golden Pyramids, and 87.928%
// No set chips required
ROM_START( goldpyra )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x08ef13 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xd3126f08
	        Calculated Checksum 0xd3126f08  (OK)
	    0x08ef14-0x1aca3b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x08ef14-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg120699.u7",  0x000000, 0x80000, CRC(19d21119) SHA1(de809e098a676f3d5f2c2750c5f7c97ee4c836a7) )
	ROM_LOAD32_WORD( "ahg120699.u11", 0x000002, 0x80000, CRC(0f585009) SHA1(2e93caba45d5a79db2d7fd3a15da432afe05e59c) )
	ROM_LOAD32_WORD( "ahg120699.u8",  0x100000, 0x80000, CRC(df1ffb31) SHA1(1cf9d008b1f8fdb06ba050c97dae79f272c8063c) )
	ROM_LOAD32_WORD( "ahg120699.u12", 0x100002, 0x80000, CRC(13021157) SHA1(6beb8dea66659b5ec9cbb8dcd921c30a621049a9) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100878V / 602/5 / 10 Credit Multiplier / 9 Line Multiline
// Golden Pyramid / Crown Casino / C - 19/06/98
// ROM says 'Golden Pyramid' with three trailing spaces, artwork says 'Golden Pyramids'
// Original casino BIOS is not dumped, using New Zealand 0700474V BIOS until an Australian version is dumped
ROM_START( goldpyrb )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    note, this actually contains a 2nd checksum for the game, this is the base/bios check only.

	    Checksum code found at 0x001b74
	    0x000000-0x089a2f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x5ad8a58b
	        Calculated Checksum 0x5ad8a58b  (OK)
	    0x089a30-0x1b4043 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	// these are the 'bios' for Casino games (could be moved to a different base set)
	ROM_LOAD32_WORD( "0700474v.u7",  0x000000, 0x80000, CRC(04b7dcbf) SHA1(eded1223336181bb08f9593247f1f79d96278b75) )
	ROM_LOAD32_WORD( "0700474v.u11", 0x000002, 0x80000, CRC(a89ce1b5) SHA1(411b474a111f23ebd834bea5af0bf0cf3926d590) )

	ROM_LOAD32_WORD( "0100878v.u8",  0x100000, 0x80000, CRC(c3184f1c) SHA1(3f808b465175108d48ca5b2560e4546b30a7fd72) )
	ROM_LOAD32_WORD( "0100878v.u12", 0x100002, 0x80000, CRC(acb3de77) SHA1(e0e337d6efbd6ee8e0c0ec2653c3dc0bd5741ff4) )
	ROM_LOAD32_WORD( "0100878v.u9",  0x200000, 0x80000, CRC(0a2f6903) SHA1(11fd913f8c3a677ae07c7ec50548a82c1eaf63ee) )
	ROM_LOAD32_WORD( "0100878v.u13", 0x200002, 0x80000, CRC(0df660be) SHA1(73d370d90655dada34f2b5b2209652632c34a22e) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101164V / 661 / 50 Credit Multiplier / 20 Line Multiline
// Golden Ra / NSW/ACT / A - 10/04/00
ROM_START( goldenra )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b98
	    0x000000-0x068297 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x1cc81433
	        Calculated Checksum 0x1cc81433  (OK)
	    0x068298-0x285abf is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x068298-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101164v.u7",  0x000000, 0x80000, CRC(2f75d5f7) SHA1(d7f6ecff7cf759d80733b6d3f224caa5128be0b7) )
	ROM_LOAD32_WORD( "0101164v.u11", 0x000002, 0x80000, CRC(06a871c7) SHA1(95464d74c2295196e367e34efb816acedcd71265) )
	ROM_LOAD32_WORD( "0101164v.u8",  0x100000, 0x80000, CRC(940eabd7) SHA1(8d41b3fa27c827a7671b095618ac53750e6017f6) )
	ROM_LOAD32_WORD( "0101164v.u12", 0x100002, 0x80000, CRC(21c4a2d2) SHA1(77a24a5f98aad090223d301919645b5011667c28) )
	ROM_LOAD32_WORD( "0101164v.u9",  0x200000, 0x80000, CRC(b1cac0e7) SHA1(87f393a75c09e96a7fb893a767edcc81044e4fe3) )
	ROM_LOAD32_WORD( "0101164v.u13", 0x200002, 0x80000, CRC(8f62ccc5) SHA1(5105313192ab8dfd522b921c70b8b03a8a61ac63) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 03J00241 / JB008 / Multi credit / Multi line
// Honey Pot / NSW/ACT / A - 21/11/2000
ROM_START( honeypot )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000bb8
	    0x000000-0x06f02b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x38355dbb
	        Calculated Checksum 0x38355dbb  (OK)
	    0x06f02c-0x281a27 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x06f02c-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "03j00241.u7",  0x000000, 0x80000, CRC(9c0c3a68) SHA1(39e8be61ea8e47fcb8006868518d9de464dc63da) )
	ROM_LOAD32_WORD( "03j00241.u11", 0x000002, 0x80000, CRC(c91bc9e7) SHA1(c8293afa113213e5bd9210b4c3fb0aae11019c4d) )
	ROM_LOAD32_WORD( "03j00241.u8",  0x100000, 0x80000, CRC(cc3701fd) SHA1(61419691b91c79aec3bde660895b520b9634237e) )
	ROM_LOAD32_WORD( "03j00241.u12", 0x100002, 0x80000, CRC(1f2e3714) SHA1(c4e5809a54c5a10479cbdfbaa7a51c5e36bd8655) )
	ROM_LOAD32_WORD( "03j00241.u9",  0x200000, 0x80000, CRC(906b275f) SHA1(71e32cfb97a1063cb363795dd03be0327f6ec3f0) )
	ROM_LOAD32_WORD( "03j00241.u13", 0x200002, 0x80000, CRC(d98e20eb) SHA1(6274ceee7ad49a9052b354d9dfc7e3cfa4e61017) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// BHG1455 / MV4122/3 / 5,10,20 Credit Multiplier / 9 Line Multiline
// HONKY TONK / CASH EXPRESS / Export / A - 07/08/01
// ROM says 'Cash Express' but this game is not a Hyperlink game
// Requires set chips 4.04.xx
// Variation (% and NO)
// The game shares reel graphics with Boot Scootin', but the game plays identically to Penguin Pays therefore it doesn't have the double wild feature.
ROM_START( hnktonku )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0ed4ff is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x576a26b7
	        Calculated Checksum 0x576a26b7  (OK)
	    0x0ed500-0x2f2d0b is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "bhg1455.u7",  0x000000, 0x80000, CRC(ef25e5a1) SHA1(00d7dcb852cc2ccb61c5af15108f8c374419527f) )
	ROM_LOAD32_WORD( "bhg1455.u11", 0x000002, 0x80000, CRC(737bd8c8) SHA1(0863ed66ca28ab47945d1c6012740bc559c79b0d) )
	ROM_LOAD32_WORD( "bhg1455.u8",  0x100000, 0x80000, CRC(822f408e) SHA1(c31d046a48f28de9d8b90676851c4ea5f30cc732) )
	ROM_LOAD32_WORD( "bhg1455.u12", 0x100002, 0x80000, CRC(520f051e) SHA1(c3d5058f753e72852e37dcd94c34a4caabdff161) )
	ROM_LOAD32_WORD( "bhg1455.u9",  0x200000, 0x80000, CRC(9c928891) SHA1(7fc4f38b9c06d4d5b27d246fffee3a1fd527d6ad) )
	ROM_LOAD32_WORD( "bhg1455.u13", 0x200002, 0x80000, CRC(18dd37fe) SHA1(432a4959886fad4f4a55de31b7cc42c51d7f2792) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100872V / 631/3 B / 25 Credit Multiplier / 20 Line Multiline
// INCA SUN / NSW/ACT / B- 03/05/99
ROM_START( incasun )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000bf8
	    0x000000-0x05f56b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x86b74381
	        Calculated Checksum 0x86b74381  (OK)
	    0x05f56c-0x23586f is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100872v.u7",  0x000000, 0x80000, CRC(180e098b) SHA1(48782c46a344dba0aaad407d0d4a432da091b0f5) )
	ROM_LOAD32_WORD( "0100872v.u11", 0x000002, 0x80000, CRC(f51b411d) SHA1(fbbd587c90cd49bb36653cbd1948bc52f8396a41) )
	ROM_LOAD32_WORD( "0100872v.u8",  0x100000, 0x80000, CRC(0c19f5ec) SHA1(95d7c9308b30b5193816e95c4276829612040298) )
	ROM_LOAD32_WORD( "0100872v.u12", 0x100002, 0x80000, CRC(0fa00c41) SHA1(79139834d5437b37346322bf632904c473e3463a) )
	ROM_LOAD32_WORD( "0100872v.u9",  0x200000, 0x80000, CRC(c82da820) SHA1(98a2710b1f793a7ee1070f89c66d49ce55e4156e) )
	ROM_LOAD32_WORD( "0100872v.u13", 0x200002, 0x80000, CRC(00407593) SHA1(4c759fe3267b1782ae84d8ed9134295dfaa0faaf) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100872V / 631/3 B / 25 Credit Multiplier / 20 Line Multiline
// INCA SUN / NSW/ACT / B- 03/05/99
// SHOW PROGRAM
ROM_START( incasunsp )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	/*
	    Checksum code found at 0x000bf8
	    0x000000-0x05f70f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x1de6e2c7
	        Calculated Checksum 0x1de6e2c7  (OK)
	    0x05f710-0x235a13 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_LOAD32_WORD( "sp__0100872v.u7",  0x000000, 0x80000, CRC(62919753) SHA1(0f0d186260a64b8b45671f68abf497586264793e) )
	ROM_LOAD32_WORD( "sp__0100872v.u11", 0x000002, 0x80000, CRC(f221ac71) SHA1(c2c1f8703e9a41e5c4d5ebfeac57e220a64e9657) )
	ROM_LOAD32_WORD( "sp__0100872v.u8",  0x100000, 0x80000, CRC(6610599f) SHA1(6d787ae58e2de2b3379a25f394c15434d4e2a8c1) )
	ROM_LOAD32_WORD( "sp__0100872v.u12", 0x100002, 0x80000, CRC(6633e701) SHA1(02e691c7d18901e70bf8c4e4aa6f856e153f05d4) )
	ROM_LOAD32_WORD( "sp__0100872v.u9",  0x200000, 0x80000, CRC(b6035aa7) SHA1(e96e802cda6f20caf523203f2032a88488bdfb65) )
	ROM_LOAD32_WORD( "sp__0100872v.u13", 0x200002, 0x80000, CRC(6d66c6b4) SHA1(2106f2ede58bd4d09334e32a1553f02a154bb767) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101108V / MV4113 / 5 Credit Multiplier/20 Line Multiline
// Inca Sun / New Zealand / A- 6/3/00
ROM_START( incasunnz )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found (uses different startup sequence)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101108v.u7",  0x000000, 0x80000, CRC(1e7be5ca) SHA1(333b7665fab8f60fb60e9d3b44de96725763ca17) )
	ROM_LOAD32_WORD( "0101108v.u11", 0x000002, 0x80000, CRC(2ff86b76) SHA1(c491ca19320bd3e15199b3ca1fcf36a70e386daa) )
	ROM_LOAD32_WORD( "0101108v.u8",  0x100000, 0x80000, CRC(3eb64fc9) SHA1(31f7d56443091da211c45dddb97375305c3cfeae) )
	ROM_LOAD32_WORD( "0101108v.u12", 0x100002, 0x80000, CRC(d91114c5) SHA1(fa88c70d81ff5e4df539b873803376e79eb6a479) )
	ROM_LOAD32_WORD( "0101108v.u9",  0x200000, 0x80000, CRC(6da340db) SHA1(4d7528aa27561185a7d53a0c44a4e95e40aadc26) )
	ROM_LOAD32_WORD( "0101108v.u13", 0x200002, 0x80000, CRC(472f4097) SHA1(5ebe72b138cdc67989db17c82979eeddc60a081e) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// CHG1458 / MV4130/3 / 20 Line Multiline / 5, 10, 25, 50 Credit Multiplier
// Inca Sun / Export / A - 05/09/00
// Requires set chips 4.03.xx
ROM_START( incasunu )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0e847f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xef64419b
	        Calculated Checksum 0xef64419b  (OK)
	    0x0e8480-0x2e9023 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "chg1458.u7",  0x000000, 0x80000, CRC(20c78b79) SHA1(d7402ff89160f25c9f4f67bbf688621d4ce22205) )
	ROM_LOAD32_WORD( "chg1458.u11", 0x000002, 0x80000, CRC(12304203) SHA1(eea44382a2711ceb6661949692e5b5a742dd0761) )
	ROM_LOAD32_WORD( "chg1458.u8",  0x100000, 0x80000, CRC(4618ecd4) SHA1(0ac6bfd6ec2bda5f4d474769f35bc81431f25c2a) )
	ROM_LOAD32_WORD( "chg1458.u12", 0x100002, 0x80000, CRC(b07d450c) SHA1(432fb4728480b76018b22e971027efb23deb7ff3) )
	ROM_LOAD32_WORD( "chg1458.u9",  0x200000, 0x80000, CRC(2f909651) SHA1(b4beaebbb20e879a1e23683a9001cbbd2ebf70c4) )
	ROM_LOAD32_WORD( "chg1458.u13", 0x200002, 0x80000, CRC(2e573a8d) SHA1(aa8ac4f4a427829f0a5929273c618edb4ecf7b36) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )

	ROM_REGION16_BE( 0x100, "eeprom0", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom0", 0x000000, 0x000100, CRC(fea8a821) SHA1(c744cac6af7621524fc3a2b0a9a135a32b33c81b) )

	ROM_REGION16_BE( 0x100, "eeprom1", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom1", 0x000000, 0x000100, CRC(b3efdb60) SHA1(f219175019b7237f1e2d132f36803097f2a1d174) )

	ROM_REGION( 0x80000, "nvram", 0 )
	ROM_LOAD( "nvram", 0x000000, 0x080000, CRC(a68e890e) SHA1(8ab087a09cfee8d3e2d84b1003b6798c7223be03) )

	ROM_REGION( 0x20, "rtc", 0 )
	ROM_LOAD( "rtc", 0x000000, 0x00001f, CRC(6909acb0) SHA1(6a4589599cd1c477e916474e7b029e9a4e92019b) )
ROM_END


// DHG1577 / MV4130/3 / 20 Line Multiline / 5, 10, 25, 50 Credit Multiplier
// Inca Sun / Export / A - 05/09/00
// Same strings as incasunu but different version
// Requires set chips 4.04.xx
ROM_START( incasunua )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0ec3a7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x24e1ed00
	        Calculated Checksum 0x24e1ed00  (OK)
	    0x0ec3a8-0x2ecf4b is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "dhg1577.u7",  0x000000, 0x80000, CRC(e31e3c81) SHA1(56b00aae1b9385686530e089cad05308e0c0751d) )
	ROM_LOAD32_WORD( "dhg1577.u11", 0x000002, 0x80000, CRC(68d58ad3) SHA1(c521da10b719250e3f115357c6e4ff51377d4335) )
	ROM_LOAD32_WORD( "dhg1577.u8",  0x100000, 0x80000, CRC(475db842) SHA1(3c49fbf45a256222e6a5a0343a7dc96386bb8897) )
	ROM_LOAD32_WORD( "dhg1577.u12", 0x100002, 0x80000, CRC(f1be432e) SHA1(ed4a064e4e8200126bda3f1c25ac5075014c9f94) )
	ROM_LOAD32_WORD( "dhg1577.u9",  0x200000, 0x80000, CRC(a8956bf5) SHA1(d444a3a64968a6275b38babf23089c7d186f30e6) )
	ROM_LOAD32_WORD( "dhg1577.u13", 0x200002, 0x80000, CRC(f7c30ef9) SHA1(44d7213e1220d6b89715408d009ef1484de2696f) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 01J01946 / 631/3 B / 25 Credit Multiplier / 20 Line Multiline
// INCA SUN / VENEZUELA / B- 03/05/99
// Game is in Spanish, however audit mode is in English
ROM_START( incasunv )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000c10
	    0x000000-0x082163 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xd9d82a51
	        Calculated Checksum 0xd9d82a51  (OK)
	    0x082164-0x258447 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "01j01946.u7",  0x000000, 0x80000, CRC(c6bdbc96) SHA1(ee83b6b4aaa9538ca04f136a4523fd520a24a878) )
	ROM_LOAD32_WORD( "01j01946.u11", 0x000002, 0x80000, CRC(15be6a5d) SHA1(000218c13e1e3fccdc0565e496a1fd13cb32f6b1) )
	ROM_LOAD32_WORD( "01j01946.u8",  0x100000, 0x80000, CRC(e26bd9c7) SHA1(e5facaa85c4c72d070b1d5000b80a333122c4fbc) )
	ROM_LOAD32_WORD( "01j01946.u12", 0x100002, 0x80000, CRC(a9941a3b) SHA1(14868686aa0aa3d966ffbaef2593a11a57a43b05) )
	ROM_LOAD32_WORD( "01j01946.u9",  0x200000, 0x80000, CRC(90e5a52b) SHA1(b2c334126128f9569846655d78f1641a36aec739) )
	ROM_LOAD32_WORD( "01j01946.u13", 0x200002, 0x80000, CRC(0efdd830) SHA1(de0be1cd3f691299f48d7a184df6bb59fef22de2) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100845V / 628/1 / 25 Credit Multiway / 20 Credit Multiplier
// Indian Dreaming / Local / B - 15/12/98
ROM_START( indrema5 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ba8
	    0x000000-0x06323f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x965e92e4
	        Calculated Checksum 0x965e92e4  (OK)
	    0x063240-0x1cd2d3 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x063240-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100845v.u7",  0x000000, 0x80000, CRC(0c924a3e) SHA1(499b4ae601e53173e3ba5f400a40e5ae7bbaa043) )
	ROM_LOAD32_WORD( "0100845v.u11", 0x000002, 0x80000, CRC(e371dc0f) SHA1(a01ab7fb63a19c144f2c465ecdfc042695124bdf) )
	ROM_LOAD32_WORD( "0100845v.u8",  0x100000, 0x80000, CRC(1c6bfb47) SHA1(7f751cb499a6185a0ab64eeec511583ceeee6ee8) )
	ROM_LOAD32_WORD( "0100845v.u12", 0x100002, 0x80000, CRC(4bbe67f6) SHA1(928f88387da66697f1de54f086531f600f80a15e) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101340V / 628/3 / 10 Credit Multiplier / 20 Line MultiLine
// INDIAN DREAMING / QLD / A - 02/05/00
ROM_START( indrema5ql )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ad0
	    0x000000-0x081823 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xa70d8e67
	        Calculated Checksum 0xa70d8e67  (OK)
	    0x081824-0x20612b is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101340v.u7",  0x000000, 0x80000, CRC(60b93091) SHA1(6d4953b8312866f952b9658a680e8827bf2b5e57) )
	ROM_LOAD32_WORD( "0101340v.u11", 0x000002, 0x80000, CRC(34125c7d) SHA1(c6e57346f2c53a04e858b01132abcb4d91403822) )
	ROM_LOAD32_WORD( "0101340v.u8",  0x100000, 0x80000, CRC(de715129) SHA1(30c15bdfc9ee5c45e04b9f9eb614be89a911260b) )
	ROM_LOAD32_WORD( "0101340v.u12", 0x100002, 0x80000, CRC(48d7b043) SHA1(835d148db1e2ce7747a4410e3ceea4468355a164) )
	ROM_LOAD32_WORD( "0101340v.u9",  0x200000, 0x80000, CRC(77c94665) SHA1(48d786a323603d1315f94fef97de6f9d1311c688) )
	ROM_LOAD32_WORD( "0101340v.u13", 0x200002, 0x80000, CRC(7ec9557e) SHA1(8f143c1ee1c82d822ff66ec5b3dac03b0c911223) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100161V / 586/2 / 10 Credit Multiplier/9 Line Multiline
// Jumping Beans / Local / A - 25/1/96
ROM_START( jumpbean )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to ROMs being corrupted, all files are missing bytes consisting of 0x0D
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100161v.u7",  0x000000, 0x7fa4c, BAD_DUMP CRC(6994c968) SHA1(7896a93aeec9c2d815c49d203ca594644e5df8a6) )
	ROM_LOAD32_WORD( "0100161v.u11", 0x000002, 0x7ffda, BAD_DUMP CRC(7776e753) SHA1(3d4753513b63f48500cb289bd0fcc28d0f4ad9d8) )
	ROM_LOAD32_WORD( "0100161v.u8",  0x100000, 0x7fa52, BAD_DUMP CRC(7e69c6ba) SHA1(105bad21c78737b78f0f740a224b3f3e8b44c751) )
	ROM_LOAD32_WORD( "0100161v.u12", 0x100002, 0x7fa54, BAD_DUMP CRC(d1d6cfba) SHA1(8c8ee5a97bc3c8cd21cd291701cebf214ca388f3) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100383V / 586/6 / 25 Credit Multiplier / 20 Line Multiline
// JUMPIN' JOEY 500cm / NSW/ACT / C - 13/11/96
ROM_START( jumpjoey )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ae8
	    0x000000-0x0562cb is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x4ac8a1e5
	        Calculated Checksum 0x4ac8a1e5  (OK)
	    0x0562cc-0x1cb767 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0562cc-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100383v.u7",  0x000000, 0x80000, CRC(9ce4ce4a) SHA1(cde42dc82432baba4c6471cb57be89c0f27ed520) )
	ROM_LOAD32_WORD( "0100383v.u11", 0x000002, 0x80000, CRC(b67419d0) SHA1(3107d3fd852fab15e8a72850c984b74e522d91cc) )
	ROM_LOAD32_WORD( "0100383v.u8",  0x100000, 0x80000, CRC(94b94149) SHA1(239b510c3ebe9114c27cd7b85fb8f0f5b7b55009) )
	ROM_LOAD32_WORD( "0100383v.u12", 0x100002, 0x80000, CRC(defce2e9) SHA1(95f88f8647c52f99dceb4920780696d7f7c1c24b) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200240V / 566/3 / 5 Credit Multiplier/9 Line Multiline
// Jungle Juice / Crown / F - 06/03/96
// ROM says 'Crown' as region (Crown Casino), but game was from Dunedin Casino with New Zealand base chips
ROM_START( jungjuic )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    note, this actually contains a 2nd checksum for the game, this is the base/bios check only.

	    Checksum code found at 0x001b74
	    0x000000-0x089a2f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x5ad8a58b
	        Calculated Checksum 0x5ad8a58b  (OK)
	    0x089a30-0x1b4043 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	// these are the 'bios' for Casino games (could be moved to a different base set)
	ROM_LOAD32_WORD( "0700474v.u7",  0x000000, 0x80000, CRC(04b7dcbf) SHA1(eded1223336181bb08f9593247f1f79d96278b75) )
	ROM_LOAD32_WORD( "0700474v.u11", 0x000002, 0x80000, CRC(a89ce1b5) SHA1(411b474a111f23ebd834bea5af0bf0cf3926d590) )

	ROM_LOAD32_WORD( "0200240v.u8",  0x100000, 0x80000, CRC(10c61ff7) SHA1(86d17cf2492612c3a6284a1c8e41a67a5199c0eb) )
	ROM_LOAD32_WORD( "0200240v.u12", 0x100002, 0x80000, CRC(ffa3d0ba) SHA1(e60e01d4d425aea483387fa2f9ae5bb69b80f829) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200536V / 613/6 / 10 Credit Multiplier/20 Line Multiline
// King Galah / Local / A - 21/07/95
// ROM says 1995 but artwork says 1997; game has a 1998+ style denomination sign
ROM_START( kgalah )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b28
	    0x000000-0x05af27 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xa4ff4d2a
	        Calculated Checksum 0xa4ff4d2a  (OK)
	    0x05af28-0x1b3e9f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05af28-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200536v.u7",  0x000000, 0x80000, CRC(9333543a) SHA1(dbbd59de046c35e70e71836b342eb5ecf4799575) )
	ROM_LOAD32_WORD( "0200536v.u11", 0x000002, 0x80000, CRC(2b52a5e2) SHA1(0c852c6672a46f269f1407db0dd1825a51f242cc) )
	ROM_LOAD32_WORD( "0200536v.u8",  0x100000, 0x80000, CRC(08bea3b7) SHA1(9a5d8cf60c9643061dede926a04006a9a674fd8f) )
	ROM_LOAD32_WORD( "0200536v.u12", 0x100002, 0x80000, CRC(15d5bfb4) SHA1(7c48dabfd83cc30fe2ffd0b4de63fbc9dc56ee2f) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100536V / 613 / 10 Credit Multiplier/20 Line Multiline
// King Galah / Local / A - 21/07/95
// ROM says 1995 but artwork says 1997; game has the newer style music introduced in 1997
ROM_START( kgalaha )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b28
	    0x000000-0x058863 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xba6c9852
	        Calculated Checksum 0xba6c9852  (OK)
	    0x058864-0x1b0b4f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x058864-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100536v.u7",  0x000000, 0x80000, CRC(0969274b) SHA1(fc9d667d963d70a563ba7acdeaa6d728f3bee9d9) )
	ROM_LOAD32_WORD( "0100536v.u11", 0x000002, 0x80000, CRC(aa7d345d) SHA1(b451ad64d5c33f4ed0fdb693e9c3be6b61093bd8) )
	ROM_LOAD32_WORD( "0100536v.u8",  0x100000, 0x80000, CRC(2f397873) SHA1(7679b324fd6c944e31a255f0c65a94a2e78fb57e) )
	ROM_LOAD32_WORD( "0100536v.u12", 0x100002, 0x80000, CRC(ddde1739) SHA1(d2dec30baef8b43b2f3bc1d572353b5afe01be4b) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1625 / MV4112/2 / 5,10,25,50 Credit Multiplier / 20 Line Multiline
// KING GALAH / Export / B - 07/02/01
// Requires set chips 4.04.xx
// Variation (% and NO):
// Cash Express Hyperlink game, but can also run standalone without progressive jackpot
ROM_START( kgalahce )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0f19b3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x6f1ff98e
	        Calculated Checksum 0x6f1ff98e  (OK)
	    0x0f19b4-0x3b1437 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1625.u7",  0x000000, 0x80000, CRC(2adcfd60) SHA1(bf819be7665df421435b997259dee0668482a837) )
	ROM_LOAD32_WORD( "ahg1625.u11", 0x000002, 0x80000, CRC(4820a290) SHA1(8d073694365f4b0a85e3abe18cc70fd738db6e2c) )
	ROM_LOAD32_WORD( "ahg1625.u8",  0x100000, 0x80000, CRC(0e067eab) SHA1(b258b62ca637ee1bc14ccf19b2d337324c0f1b8f) )
	ROM_LOAD32_WORD( "ahg1625.u12", 0x100002, 0x80000, CRC(08a30954) SHA1(dfd160b0341a8487649e7167f2772dd060f05723) )
	ROM_LOAD32_WORD( "ahg1625.u9",  0x200000, 0x80000, CRC(b2b29dce) SHA1(ae4bdec2c4c48036fca5672fd3cb1e0dfda76685) )
	ROM_LOAD32_WORD( "ahg1625.u13", 0x200002, 0x80000, CRC(a35444cc) SHA1(faa0b3edc2acb313d1ce78c7b7dd14b8de465231) )
	ROM_LOAD32_WORD( "ahg1625.u10", 0x300000, 0x80000, CRC(f9dde22f) SHA1(b321c927a958d0e20e29cf4e12cea1c8bd06a893) )
	ROM_LOAD32_WORD( "ahg1625.u14", 0x300002, 0x80000, CRC(499419db) SHA1(6df8c7a9a69d60e746eb266127647327d4b88919) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200024V / 540/3 / 10 Credit Multiplier/5 Line Multiline
// K. G. Bird / Local / D - 10/10/94
ROM_START( kgbirda5 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ae0
	    0x000000-0x0435af is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xe31fbb21
	        Calculated Checksum 0xe31fbb21  (OK)
	    0x0435b0-0x1df8a7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0435b0-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200024v.u7",  0x000000, 0x80000, CRC(90aefddc) SHA1(610b850c1d3e882c4df9e0a09a056b0c97341a19) )
	ROM_LOAD32_WORD( "0200024v.u11", 0x000002, 0x80000, CRC(52791ad8) SHA1(6e4cf553b355f03ef69ef3c4e2816bbd0cbe6599) )
	ROM_LOAD32_WORD( "0200024v.u8",  0x100000, 0x80000, CRC(c0477ae3) SHA1(5005944b8b28553dd959192d614be7f1b6228a30) )
	ROM_LOAD32_WORD( "0200024v.u12", 0x100002, 0x80000, CRC(df176c5a) SHA1(dcaecdefb7c880b9425a6445dbed969968fe3d1c) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// CHG1573 / MV4137 / 5,10,25,50 Credit Multiplier / 20 Line Multiline
// Koala Mint / Export / A - 12/09/01
// Requires set chips 4.04.xx
ROM_START( koalamnt )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0ec32b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x5e570341
	        Calculated Checksum 0x5e570341  (OK)
	    0x0ec32c-0x34ebdf is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "chg1573.u7",  0x000000, 0x80000, CRC(6e3ec3ae) SHA1(057650b8d89e93bcbaacf06e6487264fe19a1aa0) )
	ROM_LOAD32_WORD( "chg1573.u11", 0x000002, 0x80000, CRC(1ce786ca) SHA1(70539191d97d04838a706dd0bdd7969b1d3a0002) )
	ROM_LOAD32_WORD( "chg1573.u8",  0x100000, 0x80000, CRC(070920b4) SHA1(568db78a6eae1e2c7dd9b6629117bce791cce681) )  // base
	ROM_LOAD32_WORD( "chg1573.u12", 0x100002, 0x80000, CRC(9c4a36a6) SHA1(b0e44974933334fa481be2bbe41c138e2aea4a98) )  // base
	ROM_LOAD32_WORD( "chg1573.u9",  0x200000, 0x80000, CRC(3e4fbd53) SHA1(c014ee6415a5129457ab00e29cc11e240fe5da96) )  // base
	ROM_LOAD32_WORD( "chg1573.u13", 0x200002, 0x80000, CRC(7d9041cd) SHA1(ce022b0499b5ad585875a94a78aa018f3493dfb9) )  // base
	ROM_LOAD32_WORD( "chg1573.u10", 0x300000, 0x80000, CRC(a0fb61fe) SHA1(2a77ed082bc6829905f83a3cb3c4c120fa4ba0f9) )  // base
	ROM_LOAD32_WORD( "chg1573.u14", 0x300002, 0x80000, CRC(5e4776e9) SHA1(d44851cbfaa054cd5675a841a3089a8f4fdc8421) )  // base

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100677V / 614/2 / 10 Credit Multiplier/20 Line Multiline
// Kooka Bucks 200cm / NSW/ACT / A - 03/04/98
ROM_START( kookabuk )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x061857 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xf03ce7cb
	        Calculated Checksum 0xf03ce7cb  (OK)
	    0x061858-0x1a2757 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x061858-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100677v.u7",  0x000000, 0x80000, CRC(b2fdf0e8) SHA1(0dd002cfad2fa4f217a0c67066d098f4cd3ba319) )
	ROM_LOAD32_WORD( "0100677v.u11", 0x000002, 0x80000, CRC(e8ab9afc) SHA1(4c3beefeafc6ac9d4538254bb5e01c12b35db922) )
	ROM_LOAD32_WORD( "0100677v.u8",  0x100000, 0x80000, CRC(f5a45c57) SHA1(a452a7359af6d5fde2c37946ee68807152f07d39) )
	ROM_LOAD32_WORD( "0100677v.u12", 0x100002, 0x80000, CRC(b2f2fd15) SHA1(9614f3ae6e82a40ecf44090d0b8d7bd8b6b1f830) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// BHG1204 / MV4114/1 / 3,5,10,20,25,50 Credit Multiplier / 20 Line Multiline
// Keep Your Hat On / Export / B - 08/05/2000
// Requires set chips 4.01.xx
// Variation (% and NO)
// Touchscreen game
ROM_START( kyhatonu )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0a6927 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x57cc5e5a
	        Calculated Checksum 0x57cc5e5a  (OK)
	    0x0a6928-0x39cd8f is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "bhg1204.u7",  0x000000, 0x80000, CRC(bc365ca9) SHA1(f5bb4fc0b2eaed1274b21bc4762b6914ce621d8a) )
	ROM_LOAD32_WORD( "bhg1204.u11", 0x000002, 0x80000, CRC(14376c1e) SHA1(616549215f35c4f5a8c4243f84290dd8b3fae5df) )
	ROM_LOAD32_WORD( "bhg1204.u8",  0x100000, 0x80000, CRC(29081b50) SHA1(0c3990b11c9e249829babc0631840e982a387709) )
	ROM_LOAD32_WORD( "bhg1204.u12", 0x100002, 0x80000, CRC(aba9029c) SHA1(2fe65670cf1ec7a5caab770ad9af6ad3af070e53) )
	ROM_LOAD32_WORD( "bhg1204.u9",  0x200000, 0x80000, CRC(45a025db) SHA1(ea1f64a6c0272bb84ebdb0975f2d8198b8b5550a) )
	ROM_LOAD32_WORD( "bhg1204.u13", 0x200002, 0x80000, CRC(fd4b24c4) SHA1(97db7899c61050c7580af67177c7fe2c8b6116aa) )
	ROM_LOAD32_WORD( "bhg1204.u10", 0x300000, 0x80000, CRC(164ac0dd) SHA1(f6168ab76455ad29df6a4a43eea65d8c644d295a) )
	ROM_LOAD32_WORD( "bhg1204.u14", 0x300002, 0x80000, CRC(490a9eb5) SHA1(30df13960614f82deac5426ff754885ffb429603) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100473V / 599/3 / 25 Credit Multiplier / 20 Line Multiline
// Loco Loot / Local / C - 17/06/97
ROM_START( locoloot )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b20
	    0x000000-0x05633b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x02238afa
	        Calculated Checksum 0x02238afa  (OK)
	    0x05633c-0x0bfbef is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100473v.u7",  0x000000, 0x80000, CRC(fd9685ed) SHA1(c5e60cdc0a42c63f18ba33e7d8ea15a545031eb6) )
	ROM_LOAD32_WORD( "0100473v.u11", 0x000002, 0x80000, CRC(0c0c2697) SHA1(0fc1dec8fba488a4b59c81b5bab7a11d62be2599) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100472V / 599/2 / 20 Credit Multiplier / 9 Line Multiline
// Loco Loot / Local / C - 17/06/97
ROM_START( locoloota )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b20
	    0x000000-0x055e93 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xafd2e94d
	        Calculated Checksum 0xafd2e94d  (OK)
	    0x055e94-0x0bbf23 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100472v.u7",  0x000000, 0x80000, CRC(4f02763c) SHA1(302cea5fb157f65fc907f123ef42a0a38cc707ac) )
	ROM_LOAD32_WORD( "0100472v.u11", 0x000002, 0x80000, CRC(21332a1a) SHA1(76a4c30d1c9624984175e9bd117c68c9204f01df) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0600725V / MV4064 / 5 Credit Multiplier / 20 Line Multiline
// Loco Loot / New Zealand / A - 8/7/98
ROM_START( locolootnz )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found (uses different startup sequence)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0600725v.u7",  0x000000, 0x80000, CRC(164dd049) SHA1(c99c56af72cb1eb69591cb8f7bacbd06bdb6494d) )
	ROM_LOAD32_WORD( "0600725v.u11", 0x000002, 0x80000, CRC(93b0bde3) SHA1(06cb79482f8a94e1a504eead9cdf6da41cba1fb9) )
	ROM_LOAD32_WORD( "0600725v.u8",  0x100000, 0x80000, CRC(8cb449ce) SHA1(2372cf126c2c95d9637b0a761dfc7ea223f0aa54) )
	ROM_LOAD32_WORD( "0600725v.u12", 0x100002, 0x80000, CRC(29f03505) SHA1(c173167f43cc2eef0e063118e03bc37a87188391) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1513 / MV4134 / 5,10,25,50 Credit Multiplier / 20 Line Multiline
// Loco Loot / Export / A - 30/07/01
// Requires set chips 4.04.xx
ROM_START( locolootu )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0e94fb is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xbd28f614
	        Calculated Checksum 0xbd28f614  (OK)
	    0x0e94fc-0x170993 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0e94fc-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1513.u7",  0x000000, 0x80000, CRC(16854250) SHA1(a3b6e112dcce38310ca13eb9e9851901ee213fcf) )
	ROM_LOAD32_WORD( "ahg1513.u11", 0x000002, 0x80000, CRC(0abe509a) SHA1(a5d0b870390e7f4b38d7685c09c079acb198dd42) )
	ROM_LOAD32_WORD( "ahg1513.u8",  0x100000, 0x80000, CRC(bd9d2ad7) SHA1(5e5eaf2521473246fb54175f88521b43b9b86f9f) )
	ROM_LOAD32_WORD( "ahg1513.u12", 0x100002, 0x80000, CRC(5bf7b4b1) SHA1(2282ec8d0ddd1aea715005fba2171468831513f4) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100587V / 621 / 3 Credit Multiplier/3 Line Multiline
// Lone Wolf / Local / A - 29/10/97
ROM_START( lonewolf )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b48
	    0x000000-0x0580f3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x424e42b6
	        Calculated Checksum 0x424e42b6  (OK)
	    0x0580f4-0x0df6b7 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100587v.u7",  0x000000, 0x80000, CRC(15024eae) SHA1(7101125aa8531c75f9d80fe357013d09dbb0fec9) )
	ROM_LOAD32_WORD( "0100587v.u11", 0x000002, 0x80000, CRC(0ed6fb6b) SHA1(a2baa4154fe762e2c1b40a97b2d27265df8b5dab) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0300109V / 570/6 / 20 Credit Multiplier/9 Line Multiline
// Lucky Clover / Local / A - 12/10/95
ROM_START( luckyclo )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ae8
	    0x000000-0x055e07 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xd747f16a
	        Calculated Checksum 0xd747f16a  (OK)
	    0x055e08-0x0f9a0f is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0300109v.u7",  0x000000, 0x80000, CRC(175db8bb) SHA1(7c1e60c41c8b1cc73cd3476c742d7ce16837fa1b) )
	ROM_LOAD32_WORD( "0300109v.u11", 0x000002, 0x80000, CRC(4be1cdef) SHA1(8633077a6ddde80b2e7a3c4439ccb5a3f2b83695) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1211-99 / MV4033 / 10 Credit Multiplier / 9 Line Multiline
// Magic Garden / Export / B - 10/02/97
// Marked as AHG1211 and 88.26%
// No set chips required
ROM_START( mgarden )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0a522b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x8b0f5dae
	        Calculated Checksum 0x8b0f5dae  (OK)
	    0x0a522c-0x15dbd7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0a522c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg121199.u7",  0x000000, 0x80000, CRC(12b9aaf2) SHA1(1cc86a0c8daefc1e5b5a4ff710319a85f573f526) )
	ROM_LOAD32_WORD( "ahg121199.u11", 0x000002, 0x80000, CRC(b6095f2c) SHA1(d36ceaa0e6545f594f6a50ddfa71d84ca7128f3b) )
	ROM_LOAD32_WORD( "ahg121199.u8",  0x100000, 0x80000, CRC(a315ca28) SHA1(0309789362a945d592ee2eda912e4fc2e6ea5be6) )
	ROM_LOAD32_WORD( "ahg121199.u12", 0x100002, 0x80000, CRC(4b252c2c) SHA1(8be41fb2b8f8d2829c18ea123a02f3e61c136206) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1549 / MV4115_1 / 5, 10, 25, 50 Credit Multiplier / 20 Line Multiline
// Magic Mask / Export / A - 09/05/2000
// Requires set chips 4.04.xx
// Touchscreen game
ROM_START( magimask )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0e95fb is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x71464677
	        Calculated Checksum 0x71464677  (OK)
	    0x0e95fc-0x1e5983 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0e95fc-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1549.u7",  0x000000, 0x80000, CRC(a77fe800) SHA1(0e8fb392d5213c7512900c1f0fd34e795ba73e9f) )
	ROM_LOAD32_WORD( "ahg1549.u11", 0x000002, 0x80000, CRC(321ed7c0) SHA1(4a7913c0edfeb0f3ad6b292919ad1a8b427e936f) )
	ROM_LOAD32_WORD( "ahg1549.u8",  0x100000, 0x80000, CRC(b4a0334e) SHA1(7dbe781a20a9a40149658f9df35c8d39039ac70c) )
	ROM_LOAD32_WORD( "ahg1549.u12", 0x100002, 0x80000, CRC(d008deab) SHA1(fd544767356bfdf44ec4af7218c9f2990581e620) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1548 / MV4115 / 5,10,20 Credit Multiplier / 9 Line Multiline
// Magic Mask / Export / A - 09/05/2000
// Requires set chips 4.04.xx
// Touchscreen game
ROM_START( magimaska )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0e9597 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x6610851f
	        Calculated Checksum 0x6610851f  (OK)
	    0x0e9598-0x1e591f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0e9598-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1548.u7",  0x000000, 0x80000, CRC(452a19c9) SHA1(aab1f4ccfc6cdb382f7a0e85491614cc58811a08) )
	ROM_LOAD32_WORD( "ahg1548.u11", 0x000002, 0x80000, CRC(c57601f3) SHA1(1616a424b41ad6fea6383a08d5352e8240433374) )
	ROM_LOAD32_WORD( "ahg1548.u8",  0x100000, 0x80000, CRC(607d7447) SHA1(064dbfe8b52eebe1be7a41735da3fa01eacd1686) )
	ROM_LOAD32_WORD( "ahg1548.u12", 0x100002, 0x80000, CRC(cf4cd569) SHA1(408edcd746587d249c4286f7a99f33ad94214f7c) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )

	ROM_REGION16_BE( 0x100, "eeprom0", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom0", 0x000000, 0x000100, CRC(fea8a821) SHA1(c744cac6af7621524fc3a2b0a9a135a32b33c81b) )

	ROM_REGION16_BE( 0x100, "eeprom1", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom1", 0x000000, 0x000100, CRC(a10501f9) SHA1(34fdcd16bd7dc474baadc0836e2083abaf589549) )

	ROM_REGION( 0x80000, "nvram", 0 )
	ROM_LOAD( "nvram", 0x000000, 0x080000, CRC(5365446b) SHA1(9ae7a72d0ed3e7f7523a2e0a8f0dc014c6490438) )

	ROM_REGION( 0x20, "rtc", 0 )
	ROM_LOAD( "rtc", 0x000000, 0x00001f, CRC(6909acb0) SHA1(6a4589599cd1c477e916474e7b029e9a4e92019b) )
ROM_END


// DHG1309 / MV4115 / 5,10,20 Credit Multiplier / 9 Line Multiline
// Magic Mask / Export / A - 09/05/2000
// Same strings as magimaska but different version
// All devices are 27c4002 instead of 27c4096
// Requires set chips 4.03.xx
// Touchscreen game
ROM_START( magimaskb )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0e8527 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x1d86deee
	        Calculated Checksum 0x1d86deee  (OK)
	    0x0e8528-0x1e4887 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0e8528-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "dhg1309.u7",  0x000000, 0x80000, CRC(17317eb9) SHA1(3ddb8d61f23461c3194af534928164550208bbee) )
	ROM_LOAD32_WORD( "dhg1309.u11", 0x000002, 0x80000, CRC(42af4b3f) SHA1(5d88951f77782ff3861b6550ace076662a0b45aa) )
	ROM_LOAD32_WORD( "dhg1309.u8",  0x100000, 0x80000, CRC(23aefb5a) SHA1(ba4488754794f75f53b9c81b74b6ccd992c64acc) )
	ROM_LOAD32_WORD( "dhg1309.u12", 0x100002, 0x80000, CRC(6829a7bf) SHA1(97eed83763d0ec5e753d6ad194e906b1307c4940) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )

	ROM_REGION16_BE( 0x100, "eeprom0", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom0", 0x000000, 0x000100, CRC(fea8a821) SHA1(c744cac6af7621524fc3a2b0a9a135a32b33c81b) )

	ROM_REGION16_BE( 0x100, "eeprom1", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom1", 0x000000, 0x000100, CRC(6e485bbc) SHA1(3d6c8d120c69ed2804f267c50681974f73e1ee51) )

	ROM_REGION( 0x80000, "nvram", 0 )
	ROM_LOAD( "nvram", 0x000000, 0x080000, CRC(538c7523) SHA1(1e6516b77daf855e397c1ec590e73637ce3b8406) )

	ROM_REGION( 0x20, "rtc", 0 )
	ROM_LOAD( "rtc", 0x000000, 0x00001f, CRC(6909acb0) SHA1(6a4589599cd1c477e916474e7b029e9a4e92019b) )
ROM_END


// 0300455V / 606 / 10 Credit Multiplier/9 Line Multiline
// Magic Touch / Local / A- 06/03/97
// Touchscreen game
ROM_START( magtcha5 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found (uses different startup sequence)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0300455v.u7",  0x000000, 0x80000, CRC(a1568e3b) SHA1(842c1aa3f9765f7ba9f2587cb94b6ef03c74e8b9) )
	ROM_LOAD32_WORD( "0300455v.u11", 0x000002, 0x80000, CRC(9449b7cb) SHA1(213e642f494892b0f24502eb896fd945e0267bba) )
	ROM_LOAD32_WORD( "0300455v.u8",  0x100000, 0x80000, CRC(a7d7b121) SHA1(fca49075463e4f21f5138e86889239fd20eabcac) )
	ROM_LOAD32_WORD( "0300455v.u12", 0x100002, 0x80000, CRC(0e23ac25) SHA1(fa6601b998bbc9cb4cea9ea2db73afa5f7937bf9) )
	ROM_LOAD32_WORD( "0300455v.u9",  0x200000, 0x80000, CRC(de502bba) SHA1(d40b71518ff15405c787a58643468a202fae97bd) )
	ROM_LOAD32_WORD( "0300455v.u13", 0x200002, 0x80000, CRC(5e54ed88) SHA1(5fe9a74e210bc2c1b158e7a3bb01fdcc96ea0075) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200455V / 606 / 10 Credit Multiplier/9 Line Multiline
// Magic Touch / Local / A- 06/03/97
// Same strings as magtcha5 but earlier version
// Touchscreen game
ROM_START( magtcha5a )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found (uses different startup sequence)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200455v.u7",  0x000000, 0x80000, CRC(9fa3ee86) SHA1(ce7546b8d1dbf90eb8f4f8d3255dc1c215c966a7) )
	ROM_LOAD32_WORD( "0200455v.u11", 0x000002, 0x80000, CRC(614984e4) SHA1(e95d576993e8d9c0964899a7d5556c8e62d79242) )
	ROM_LOAD32_WORD( "0200455v.u8",  0x100000, 0x80000, CRC(d7faf84d) SHA1(d2e49787d177767671fab64a723e1af619ce9ad2) )
	ROM_LOAD32_WORD( "0200455v.u12", 0x100002, 0x80000, CRC(f54c18db) SHA1(85bcc202f7425b3b7ef456c1c2db5a22648068a8) )
	ROM_LOAD32_WORD( "0200455v.u9",  0x200000, 0x80000, CRC(0e140453) SHA1(8b516fe598c7e754a471246effa1185845495640) )
	ROM_LOAD32_WORD( "0200455v.u13", 0x200002, 0x80000, CRC(cfd2a86e) SHA1(66891a1b0e85ad7146b733f4b5d806db789d8821) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100425V / 595/5 / 10 Credit Multiplier/20 Line Multiline
// Mammoth Money / Local / D - 07/04/97
ROM_START( mammothm )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000adc
	    0x000000-0x053623 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x2765fc8f
	        Calculated Checksum 0x2765fc8f  (OK)
	    0x053624-0x1b52eb is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x053624-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100425v.u7",  0x000000, 0x80000, CRC(43ef012a) SHA1(e80d15852cb3a8826cc8cee11e3036ff65d733ad) )
	ROM_LOAD32_WORD( "0100425v.u11", 0x000002, 0x80000, CRC(37b5a672) SHA1(44cc648c27476c401f7f90569b9fc0c7e6d4bf51) )
	ROM_LOAD32_WORD( "0100425v.u8",  0x100000, 0x80000, CRC(a6516f86) SHA1(2224b94b3b5c9d86163ff1d2bb439729092826ac) )
	ROM_LOAD32_WORD( "0100425v.u12", 0x100002, 0x80000, CRC(58171e9b) SHA1(7f375aeb8cabe22fcc6f61cac5ef6f72f0c99899) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 01J00101 / JB005 / Multi credit / Multi line
// Margarita Magic / NSW/ACT / A - 07/07/2000
ROM_START( marmagic )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b78
	    0x000000-0x06d93b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x59531d0a
	        Calculated Checksum 0x59531d0a  (OK)
	    0x06d93c-0x2deae3 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x06d93c-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "01j00101.u7",  0x000000, 0x80000, CRC(eee7ebaf) SHA1(bad0c08578877f84325c07d51c6ed76c40b70720) )
	ROM_LOAD32_WORD( "01j00101.u11", 0x000002, 0x80000, CRC(4901a166) SHA1(8afe6f08b4ac5c17744dff73939c4bc93124fdf1) )
	ROM_LOAD32_WORD( "01j00101.u8",  0x100000, 0x80000, CRC(b0d78efe) SHA1(bc8b345290f4d31c6553f1e2700bc8324b4eeeac) )
	ROM_LOAD32_WORD( "01j00101.u12", 0x100002, 0x80000, CRC(90ff59a8) SHA1(c9e342db2b5e8c3f45efa8496bc369385046e920) )
	ROM_LOAD32_WORD( "01j00101.u9",  0x200000, 0x80000, CRC(1f0ca910) SHA1(be7a2f395eae09a29faf99ba34551fbc38f20fdb) )
	ROM_LOAD32_WORD( "01j00101.u13", 0x200002, 0x80000, CRC(3f702945) SHA1(a6c9a848d059c1e564fdc5a65bf8c9600853edfa) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// EHG1558 / US003 / Multi credit / Multi line
// Margarita Magic / NSW/ACT / A - 07/07/2000
// ROM has NSW/ACT strings but it is for US platform
// Requires set chips 4.04.xx
ROM_START( marmagicu )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0ed9f3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xe8864023
	        Calculated Checksum 0xe8864023  (OK)
	    0x0ed9f4-0x319ef3 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ehg1558.u7",  0x000000, 0x80000, CRC(7c2602ae) SHA1(f5a13154448041575e4bea1399ed0a5a0888d493) )
	ROM_LOAD32_WORD( "ehg1558.u11", 0x000002, 0x80000, CRC(4fe3b18a) SHA1(cc36eddd264de1a088c16e1b741168130d895bd7) )
	ROM_LOAD32_WORD( "ehg1558.u8",  0x100000, 0x80000, CRC(c9ff4bd8) SHA1(5766d7c39e753fb2a2412a41338dcfd4e31b642b) )
	ROM_LOAD32_WORD( "ehg1558.u12", 0x100002, 0x80000, CRC(d00cd217) SHA1(2be7c1ef6c0a39d1c7ed391feeaf0f42a6471bae) )
	ROM_LOAD32_WORD( "ehg1558.u9",  0x200000, 0x80000, CRC(879e4b14) SHA1(52d9540dd3ed51d42a31d1a7effe75e995e95330) )
	ROM_LOAD32_WORD( "ehg1558.u13", 0x200002, 0x80000, CRC(770872a1) SHA1(cd430385a3dfc080decfa2402d7dbad0db912e15) )
	ROM_LOAD32_WORD( "ehg1558.u10", 0x300000, 0x80000, CRC(4301eb40) SHA1(1a32c61df2ab04c365135378d97d18bbb0b50179) )
	ROM_LOAD32_WORD( "ehg1558.u14", 0x300002, 0x80000, CRC(a2096cb3) SHA1(c2bba35396f9c83b17d692a384b6406f3441c8f5) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// EHG1559 / US003 / Multi credit / Multi line
// Margarita Magic / NSW/ACT / A - 07/07/2000
// ROM has NSW/ACT strings but it is for US platform
// Requires set chips 4.04.xx
ROM_START( marmagicua )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0eda53 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xac229593
	        Calculated Checksum 0xac229593  (OK)
	    0x0eda54-0x2fffef is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0eda54-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ehg1559.u7",  0x000000, 0x80000, CRC(dd4e4aa2) SHA1(c061a2d4a5814d62ac655f594dbfe4b96c2f68c4) )
	ROM_LOAD32_WORD( "ehg1559.u11", 0x000002, 0x80000, CRC(f1fd2079) SHA1(9dcd73eb46300dcf682b6165638c2d22e773a117) )
	ROM_LOAD32_WORD( "ehg1559.u8",  0x100000, 0x80000, CRC(db05591e) SHA1(8af241bbd4f744c66fb78fdaf739d9c8bc2580c0) )
	ROM_LOAD32_WORD( "ehg1559.u12", 0x100002, 0x80000, CRC(cbbc99b1) SHA1(96e8d18d12a7df0242fad073faa10fa52e604cf5) )
	ROM_LOAD32_WORD( "ehg1559.u9",  0x200000, 0x80000, CRC(54861629) SHA1(75fce0f47c45b049112a7e7d61b1bb3fba3e2c20) )
	ROM_LOAD32_WORD( "ehg1559.u13", 0x200002, 0x80000, CRC(8c111896) SHA1(60d2ffe82a39ef98d2cf0141773be95381538ace) )
	ROM_LOAD32_WORD( "ehg1559.u10", 0x300000, 0x80000, CRC(3db4e373) SHA1(7150242253ae4a1c4f3211e3068f00e8b1ed51b1) )
	ROM_LOAD32_WORD( "ehg1559.u14", 0x300002, 0x80000, CRC(bdfdc0e4) SHA1(0e56f08abc0cdd9dfa5d8e51bb6fe06fa356b3b3) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0400115V / 559/2 / 10 Credit Multiplier/9 Line Multiline
// Mine, Mine, Mine / Local / D - 16/01/96
ROM_START( minemine )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000adc
	    0x000000-0x0446e3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xdd1e6087
	        Calculated Checksum 0xdd1e6087  (OK)
	    0x0446e4-0x1465a7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0446e4-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0400115v.u7",  0x000000, 0x80000, CRC(e3160af4) SHA1(e0b212aba1b39acb324ff8c2850b0f2b6999d8ae) )
	ROM_LOAD32_WORD( "0400115v.u11", 0x000002, 0x80000, CRC(3544d77f) SHA1(76a6cb7e7b9500e046d2b169a224f3e99088dcb2) )
	ROM_LOAD32_WORD( "0400115v.u8",  0x100000, 0x80000, CRC(e43f6c8c) SHA1(53f35f430a328e7b4cb86ce3227c48eebc8b4c30) )
	ROM_LOAD32_WORD( "0400115v.u12", 0x100002, 0x80000, CRC(b0607ccf) SHA1(f7b35d02170620060c8cdf90b9cb6aac86e26a52) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// VHG0416-99 / 559/2 / 10 Credit Multiplier / 9 Line Multiline
// Mine, Mine, Mine / Export / E - 14/02/96
// All devices are 27c4002 instead of 27c4096
// No set chips required
ROM_START( minemineu )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	/*
	    Checksum code found at 0x000d10
	    0x000000-0x0a7203 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x75c908a7
	        Calculated Checksum 0x75c908a7  (OK)
	    0x0a7204-0x1a0edf is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0a7204-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "vhg041699.u7",  0x000000, 0x80000, CRC(41bc3714) SHA1(5a8f7d24a6a697524af7997dcedd214fcaf48768) )
	ROM_LOAD32_WORD( "vhg041699.u11", 0x000002, 0x80000, CRC(75803b10) SHA1(2ff3d966da2992ddcc7e229d979cc1ee623b4900) )
	ROM_LOAD32_WORD( "vhg041699.u8",  0x100000, 0x80000, CRC(0a3e2baf) SHA1(b9ab989cf383cd6ea0aa1ead137558a1a6f5901d) )
	ROM_LOAD32_WORD( "vhg041699.u12", 0x100002, 0x80000, CRC(26c01532) SHA1(ec68ad44b703609c7bc27275f8d9250a16d9067c) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )

	ROM_REGION16_BE( 0x100, "eeprom0", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom0", 0x000000, 0x000100, CRC(fea8a821) SHA1(c744cac6af7621524fc3a2b0a9a135a32b33c81b) )

	ROM_REGION16_BE( 0x100, "eeprom1", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom1", 0x000000, 0x000100, CRC(8421e7c2) SHA1(fc1b07d5b7aadafc4a0f2e4dfa698e7c72340717) )

	ROM_REGION( 0x80000, "nvram", 0 )
	ROM_LOAD( "nvram", 0x000000, 0x080000, CRC(883f5023) SHA1(e526e337b5b0fc77091b4946b503b56307c390e9) )

	ROM_REGION( 0x20, "rtc", 0 )
	ROM_LOAD( "rtc", 0x000000, 0x00001f, CRC(6909acb0) SHA1(6a4589599cd1c477e916474e7b029e9a4e92019b) )
ROM_END


// NHG0416-99 / 559/2 / 10 Credit Multiplier / 9 Line Multiline
// Mine, Mine, Mine / Export / E - 14/02/96
// Variation (% and NO): 87.408% 99
// All eproms are 27C4002.
// No set chips required
ROM_START( minemineua )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d10
	    0x000000-0x0a7183 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xbb6ac5e6
	        Calculated Checksum 0xbb6ac5e6  (OK)
	    0x0a7184-0x1a0e5f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0a7184-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "nhg0416.u7",  0x000000, 0x80000, CRC(7258afb3) SHA1(238b883075a5e3cdd2c675190b269c55d08b7b13) )
	ROM_LOAD32_WORD( "nhg0416.u11", 0x000002, 0x80000, CRC(c65978b0) SHA1(e16b6a57b21b4570687676f4c6b65c463ad6e559) )
	ROM_LOAD32_WORD( "nhg0416.u8",  0x100000, 0x80000, CRC(50c4d4d8) SHA1(920e62933ffa29622d941b469dcd6c5dfc2ab438) )
	ROM_LOAD32_WORD( "nhg0416.u12", 0x100002, 0x80000, CRC(fc3f18ff) SHA1(f2afc6bdd7a702e4ac4037749613ac4a40446511) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0400469V / 607/1 / 25 Credit Multiplier/20 Line Multiline
// Money Mouse / Local / B - 08/04/97
ROM_START( monmouse )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b80
	    0x000000-0x066077 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x569023a4
	        Calculated Checksum 0x569023a4  (OK)
	    0x066078-0x1faf7b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x066078-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0400469v.u7",  0x000000, 0x80000, CRC(7f7972b6) SHA1(25991f476f55cd1eddc8e63af9c472c1d7e83481) )
	ROM_LOAD32_WORD( "0400469v.u11", 0x000002, 0x80000, CRC(ac2243ea) SHA1(27c31e5102defa4f3982875b30a67e89af40d4ff) )
	ROM_LOAD32_WORD( "0400469v.u8",  0x100000, 0x80000, CRC(a10a4bff) SHA1(e6b36542dab8a3405579b333a125a6d3fd801b50) )
	ROM_LOAD32_WORD( "0400469v.u12", 0x100002, 0x80000, CRC(72d992ed) SHA1(94560305dacbe776ddc95114ad5e5ffaa234937c) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0300469V / 607/1 / 25 Credit Multiplier/20 Line Multiline
// Money Mouse / Local / B - 08/04/97
// Same strings as monmouse but earlier version
ROM_START( monmousea )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b80
	    0x000000-0x05dc0b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xd1848057
	        Calculated Checksum 0xd1848057  (OK)
	    0x05dc0c-0x1faf7b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05dc0c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0300469v.u7",  0x000000, 0x80000, CRC(ae3ece9e) SHA1(d0124a6e9dc3770c0c8b086cd208a6baf7194d3d) )
	ROM_LOAD32_WORD( "0300469v.u11", 0x000002, 0x80000, CRC(c53acb75) SHA1(70b67e15ef04eacc7cea0077aceeb6737e753d6c) )
	ROM_LOAD32_WORD( "0300469v.u8",  0x100000, 0x80000, CRC(7643ca29) SHA1(9aa2d3cb2ddc7f47af4279679cacfaf6b55a56a1) )
	ROM_LOAD32_WORD( "0300469v.u12", 0x100002, 0x80000, CRC(2be9bce0) SHA1(3768e616e4f03f253074e1f06aa628181db9dce8) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0201397V / MV4126 / 12 Credit Multiplier/20 Line Multiline
// Money Tree / New Zealand / C- 12/04/01
ROM_START( montree )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found (uses different startup sequence)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0201397v.u7",  0x000000, 0x80000, CRC(982a0078) SHA1(163c15aebd2be623c0f2c7641360336399bc1f4f) )
	ROM_LOAD32_WORD( "0201397v.u11", 0x000002, 0x80000, CRC(7a03e436) SHA1(016e6e36f7ca6f4c3b427cf98a1415ef6aa57225) )
	ROM_LOAD32_WORD( "0201397v.u8",  0x100000, 0x80000, CRC(878b6419) SHA1(572d6a10cbf2b96e9afee4b3f32e9ad1ce7eabbb) )
	ROM_LOAD32_WORD( "0201397v.u12", 0x100002, 0x80000, CRC(b11f51d0) SHA1(1579a24d470f418713334259368c7a4e35e8b5d3) )
	ROM_LOAD32_WORD( "0201397v.u9",  0x200000, 0x80000, CRC(816fda3f) SHA1(d5eb7572a93e2a4681a2b1db7f302c5528f1c1c3) )
	ROM_LOAD32_WORD( "0201397v.u13", 0x200002, 0x80000, CRC(e1f23c3f) SHA1(fe74c219c738625257fb62806e271a60075aaa07) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100294V / 595/3 / 50 Credit Multiplier/20 Line Multiline
// Mountain Money / Local / B - 11/06/96
ROM_START( mountmon )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ae4
	    0x000000-0x04ee9b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x4bb1139e
	        Calculated Checksum 0x4bb1139e  (OK)
	    0x04ee9c-0x0e3a1f is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100294v.u7",  0x000000, 0x80000, CRC(b84342af) SHA1(e27e65730ddc897b01e8875a4da3ea2d6db2b858) )
	ROM_LOAD32_WORD( "0100294v.u11", 0x000002, 0x80000, CRC(4fb2a4dc) SHA1(23895b701387f7442a31969989d21cefe2a25efd) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100289V / 595/2 / 5 Credit Multiplier/20 Line Multiline
// Mountain Money / Local / C - 11/06/96
ROM_START( mountmona )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ae4
	    0x000000-0x04eea3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xd2ec718d
	        Calculated Checksum 0xd2ec718d  (OK)
	    0x04eea4-0x0e3a27 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100289v.u7",  0x000000, 0x80000, CRC(35582166) SHA1(26bf4cd6939afe15a5c2ae940d6da921491fb401) )
	ROM_LOAD32_WORD( "0100289v.u11", 0x000002, 0x80000, CRC(565b76ff) SHA1(559d4ec4f1a727cd293d842b7f777c99dcf488bc) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1629 / MV4108/5 / 5,10,25,50 Credit Multiplier / 20 Line Multiline
// Mountain Money / Export / A - 10/03/01
// Cash Express Hyperlink game, but can also run standalone without progressive jackpot
ROM_START( mountmonce )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to ROMs being corrupted, all files except U10 are missing bytes consisting of 0x0D
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1629.u7",  0x000000, 0x7efa7, BAD_DUMP CRC(8e5b5354) SHA1(519c5af995d75c3035c0a3832956d94a989163de) )
	ROM_LOAD32_WORD( "ahg1629.u11", 0x000002, 0x7ff5b, BAD_DUMP CRC(9ccf8088) SHA1(cc0e2a2e50041e412d9fd4dea8e50b5aa3b94122) )
	ROM_LOAD32_WORD( "ahg1629.u8",  0x100000, 0x7d02f, BAD_DUMP CRC(32bd1b30) SHA1(3caf8b69a9f5fdf5f472a4ce83bf9f2e9c9af75c) )
	ROM_LOAD32_WORD( "ahg1629.u12", 0x100002, 0x7d068, BAD_DUMP CRC(6c76f501) SHA1(7420748f4ad7d7586c1cdc7676297b7a87733faf) )
	ROM_LOAD32_WORD( "ahg1629.u9",  0x200000, 0x7f6d0, BAD_DUMP CRC(50451b2a) SHA1(299252ac72d993bc79f07087130bff55bd13659d) )
	ROM_LOAD32_WORD( "ahg1629.u13", 0x200002, 0x7f739, BAD_DUMP CRC(783d8aad) SHA1(41b4d4e4d02aa8ee4a81550160750f4f36aaca09) )
	ROM_LOAD32_WORD( "ahg1629.u10", 0x300000, 0x80000, CRC(503b9a4e) SHA1(867cdecf6a721803500b1b66192ea4f61eb459da) )
	ROM_LOAD32_WORD( "ahg1629.u14", 0x300002, 0x7ffff, BAD_DUMP CRC(57f31de5) SHA1(aceb1d700c3b41e29e0abd613d59008d7dc259c9) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// BHG1465 / MV4108/5 / 5,10,25,50 Credit Multiplier / 20 Line Multiline
// Mountain Money / Export / A - 10/03/01
ROM_START( mountmonu )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to ROMs being corrupted, first 6 files are missing bytes consisting of 0x0D
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "bhg1465.u7",  0x000000, 0x7f026, BAD_DUMP CRC(9a176a6b) SHA1(a86213020f6cf0c99271ae4e5768453578ade4c3) )
	ROM_LOAD32_WORD( "bhg1465.u11", 0x000002, 0x7ff32, BAD_DUMP CRC(cfdc676a) SHA1(21179519913a1257f19a726bbfaf913018f31a9a) )
	ROM_LOAD32_WORD( "bhg1465.u8",  0x100000, 0x7d046, BAD_DUMP CRC(65950ec8) SHA1(22e558910d1ca62c36fa3c7376515268473f944f) )
	ROM_LOAD32_WORD( "bhg1465.u12", 0x100002, 0x7d079, BAD_DUMP CRC(2d34d169) SHA1(a845570cd0219801e19e3746c00b7ebedb52034b) )
	ROM_LOAD32_WORD( "bhg1465.u9",  0x200000, 0x7fc08, BAD_DUMP CRC(1dfcc079) SHA1(07fe6820cacf52fd3b25e3e58f2377eb85e5de2c) )
	ROM_LOAD32_WORD( "bhg1465.u13", 0x200002, 0x7fc71, BAD_DUMP CRC(2ce1748e) SHA1(10c78c55d76eb270db16a42b6ae43293301d70ec) )
	ROM_LOAD32_WORD( "bhg1465.u10", 0x300000, 0x80000, CRC(05e2c0e2) SHA1(c604ca86d39d337b43c32ada5fbe30f57bae47aa) )
	ROM_LOAD32_WORD( "bhg1465.u14", 0x300002, 0x80000, CRC(62f9b2af) SHA1(bddf24c7a412e911cf75316723f3139be99acbdd) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// BHG1464 / MV4108/5 / 5,10,20 Credit Multiplier / 9 Line Multiline
// Mountain Money / Export / A - 10/03/01
// Requires set chip version: 4.04.xx
// Variation (% and NO):
ROM_START( mountmonua )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0e8e57 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x0916f8a2
	        Calculated Checksum 0x0916f8a2  (OK)
	    0x0e8e58-0x3769bb is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "bhg1464.u7",  0x000000, 0x80000, CRC(92e49c10) SHA1(f4723d7ea5f3454e649e0004c6010d0ba836e5ba) )
	ROM_LOAD32_WORD( "bhg1464.u11", 0x000002, 0x80000, CRC(44b813d9) SHA1(0e62c4eb4bd8e914f419aa2e2875a5419b5bafc5) )
	ROM_LOAD32_WORD( "bhg1464.u8",  0x100000, 0x80000, CRC(01207fd3) SHA1(9e7fb7a14d41b175d8a201b5c52d08fae024571f) )
	ROM_LOAD32_WORD( "bhg1464.u12", 0x100002, 0x80000, CRC(69b71a6e) SHA1(01c7a468f7766a85235b6b5ae1e3c9748cbbcb0b) )
	ROM_LOAD32_WORD( "bhg1464.u9",  0x200000, 0x80000, CRC(1be9a7eb) SHA1(d347b87694fe389a7841ac99f0b35bc159b0a7d4) )
	ROM_LOAD32_WORD( "bhg1464.u13", 0x200002, 0x80000, CRC(fa139977) SHA1(55bbdcc0c62cc7fe7cc73aa45e45f92f41ae1f24) )
	ROM_LOAD32_WORD( "bhg1464.u10", 0x300000, 0x80000, CRC(0d225449) SHA1(4b70c2e91877b7d314e4f4616df4ef9fd49303a0) )
	ROM_LOAD32_WORD( "bhg1464.u14", 0x300002, 0x80000, CRC(5b1e5888) SHA1(4cfb0a7af2553037fed650085bbe11e3d043797b) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200956V / 386/64 / 200 Credits per Draw / 3 Draws(1 cent) / 2 Credits per Draw / 3 Draws ($1.00)
// MULTIDRAW - FREE GAMES / NSW/ACT / E - 08/05/00
ROM_START( multidrw )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b98
	    0x000000-0x07477f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xe2d3d401
	        Calculated Checksum 0xe2d3d401  (OK)
	    0x074780-0x2c5abb is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x074780-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200956v.u7",  0x000000, 0x80000, CRC(7570eb03) SHA1(0fded55ee2d12cfae96e2910c68a131cd89147a0) )
	ROM_LOAD32_WORD( "0200956v.u11", 0x000002, 0x80000, CRC(ac8503fa) SHA1(30640a9c01239173c7430a46dcd2e2b28024c0cf) )
	ROM_LOAD32_WORD( "0200956v.u8",  0x100000, 0x80000, CRC(8c54bd65) SHA1(5870558f8b96fca2c355ccc6ffc09fc4684d141c) )
	ROM_LOAD32_WORD( "0200956v.u12", 0x100002, 0x80000, CRC(cd0dfdf5) SHA1(7bcf77c1bcd023b4ab08cef329dcf39dc2ca09d6) )
	ROM_LOAD32_WORD( "0200956v.u9",  0x200000, 0x80000, CRC(10b96156) SHA1(1f89e0d8d210d2fd7e0b78b0205eb626d7c39542) )
	ROM_LOAD32_WORD( "0200956v.u13", 0x200002, 0x80000, CRC(0d6f7ec5) SHA1(0a80257eb464e50292554f45583f3d7b85de2bc3) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100275V / 595/1 / 5 Credit Multiplier/20 Line Multiline
// Mystic Garden / Local / B - 11/06/96
ROM_START( mystgard )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ae4
	    0x000000-0x04eea7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x37310f71
	        Calculated Checksum 0x37310f71  (OK)
	    0x04eea8-0x0dce17 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100275v.u7",  0x000000, 0x80000, CRC(28d15442) SHA1(ee33017f3efcf688a43ea1d7f2b74b4b9a6d2cae) )
	ROM_LOAD32_WORD( "0100275v.u11", 0x000002, 0x80000, CRC(6e618fc5) SHA1(a02e7ca2433cf8128d74792833d9708a3ba5df4b) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101503V / MV4141 / 6 Credit Multiplier/20 Line Multiline
// One For All / New Zealand / A- 28/05/01
ROM_START( one4all )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found (uses different startup sequence)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101503v.u7",  0x000000, 0x80000, CRC(edf50554) SHA1(302737220c4b7d60db77074429d6f360c55494a6) )
	ROM_LOAD32_WORD( "0101503v.u11", 0x000002, 0x80000, CRC(e2fa31a8) SHA1(595286573a4bfc6f3ee2fe57c44ae129077f3dd0) )
	ROM_LOAD32_WORD( "0101503v.u8",  0x100000, 0x80000, CRC(46aa8912) SHA1(dd874b203e4dacb868d73e5c621e0bf95c267783) )
	ROM_LOAD32_WORD( "0101503v.u12", 0x100002, 0x80000, CRC(0d02a4af) SHA1(e16d4450d5273522caf3267947ed4a54047b0894) )
	ROM_LOAD32_WORD( "0101503v.u9",  0x200000, 0x80000, CRC(c999f9a6) SHA1(49752d37259affdd74bba3c04a9e8f7f15c0ccfd) )
	ROM_LOAD32_WORD( "0101503v.u13", 0x200002, 0x80000, CRC(3b116e0d) SHA1(5df873c00c1103304c2cb77cedf05a5db83ece29) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200849V / 601/3 / 10 Credit Multiplier/10 Line Multiline
// Orchid Mist 500cm / Local / C - 03/02/99
// ROM says '10 Credit Multiplier' but game has a 50 credit multiplier
ROM_START( orchidms )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b20
	    0x000000-0x0677c7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x522d283f
	        Calculated Checksum 0x522d283f  (OK)
	    0x0677c8-0x13adcb is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0677c8-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200849v.u7",  0x000000, 0x80000, CRC(5d18ae22) SHA1(c10f7a83f51cfe75653ace8066b7dedf07e91b28) )
	ROM_LOAD32_WORD( "0200849v.u11", 0x000002, 0x80000, CRC(fe79410b) SHA1(c91a0ce0cf87db518f910e9f47cabdcb91dc5496) )
	ROM_LOAD32_WORD( "0200849v.u8",  0x100000, 0x80000, CRC(09ec43e3) SHA1(947ed0982a148e6906666378e8c82315d40237d7) )
	ROM_LOAD32_WORD( "0200849v.u12", 0x100002, 0x80000, CRC(165a762d) SHA1(8487d2e32bd2fab5a9114380ba2be6d34b097b11) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100849V / 601/3 / 10 Credit Multiplier/10 Line Multiline
// Orchid Mist 500cm / Local / C - 03/02/99
// Same strings as orchidms but earlier version
// ROM says '10 Credit Multiplier' but game has a 50 credit multiplier
ROM_START( orchidmsa )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b08
	    0x000000-0x05f753 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xf1c9125e
	        Calculated Checksum 0xf1c9125e  (OK)
	    0x05f754-0x132d57 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05f754-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100849v.u7",  0x000000, 0x80000, CRC(a0fe870c) SHA1(9283019b2615232ed5e4d72843047d27ef06c728) )
	ROM_LOAD32_WORD( "0100849v.u11", 0x000002, 0x80000, CRC(b0856963) SHA1(30ea3ddbb9d5ee403039f3d9a7cb84fb4ff0aa54) )
	ROM_LOAD32_WORD( "0100849v.u8",  0x100000, 0x80000, CRC(cfd2b025) SHA1(77d2ac9f7d7ae3c705401879d6e077fb3a03c00f) )
	ROM_LOAD32_WORD( "0100849v.u12", 0x100002, 0x80000, CRC(4b5baf9d) SHA1(2fd13cbb22aff14936cbe2da582a0aa3984ab4a2) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101241V / MV4118 / 25 Credit Multiplier/10 Line Multiline
// Orchid Mist / New Zealand / A- 3/7/00
ROM_START( orchidmsnz )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found (uses different startup sequence)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101241v.u7",  0x000000, 0x80000, CRC(e4183d54) SHA1(c5829448450dff212e76b5571fc1bf0ba681afdf) )
	ROM_LOAD32_WORD( "0101241v.u11", 0x000002, 0x80000, CRC(2623c7b7) SHA1(3601364afe75355641405c4695ef3194eeac1245) )
	ROM_LOAD32_WORD( "0101241v.u8",  0x100000, 0x80000, CRC(3ec83f06) SHA1(1a89901a5c9126646aa5425801ec4b1570072d20) )
	ROM_LOAD32_WORD( "0101241v.u12", 0x100002, 0x80000, CRC(141e2b47) SHA1(e311693d730bfff99ad75995e47606e9822ba722) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200348V / 593/2 / 10 Credit Multiplier / 9 Line Multiline
// Oscar / Local / C - 20/09/96
ROM_START( oscara5 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b80
	    0x000000-0x05d187 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xd11b30fc
	        Calculated Checksum 0xd11b30fc  (OK)
	    0x05d188-0x0e1d73 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200348v.u7",  0x000000, 0x80000, CRC(930bdc00) SHA1(36b1a289abebc7cce64e767e201d8f8f7fe80cf2) )
	ROM_LOAD32_WORD( "0200348v.u11", 0x000002, 0x80000, CRC(11394e80) SHA1(1c6e7e954a6118e04da9d761fef8ec00c46d2af8) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100348V / 593/2 / 10 Credit Multiplier / 9 Line Multiline
// Oscar / Local / C - 20/09/96
// Same strings as oscara5 but earlier version
ROM_START( oscara5a )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b80
	    0x000000-0x054093 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x5bb21f81
	        Calculated Checksum 0x5bb21f81  (OK)
	    0x054094-0x1d0ecf is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x054094-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100348v.u7",  0x000000, 0x80000, CRC(734924f1) SHA1(33d2eecd046b40f90e54c5bbaed3779ebaebbc19) )
	ROM_LOAD32_WORD( "0100348v.u11", 0x000002, 0x80000, CRC(c03b2120) SHA1(2fead5d70b58edd0f7c325d8495c61c93589a781) )
	ROM_LOAD32_WORD( "0100348v.u8",  0x100000, 0x80000, CRC(d03eb2aa) SHA1(7e6345922fc9b86c8ccbff4a452747cd09a4d28c) )
	ROM_LOAD32_WORD( "0100348v.u12", 0x100002, 0x80000, CRC(fd1c5c7b) SHA1(61b29459e39912ea3c2bf290e1f3061ce13ea648) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101046V / 594/7 / 10 Credit Multiplier/9 && 20 Line Multiline
// Panther Magic / NSW/ACT / A - 06/10/99
ROM_START( pantmag )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x06d1ff is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x50122492
	        Calculated Checksum 0x50122492  (OK)
	    0x06d200-0x195d7b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x06d200-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101046v.u7",  0x000000, 0x80000, CRC(6383899d) SHA1(df96af7cb580565715da6e78b83e7ba6832028e7) )
	ROM_LOAD32_WORD( "0101046v.u11", 0x000002, 0x80000, CRC(0914594c) SHA1(b1bc1302847e3ea3c4ed96ae17047da031e5ca1a) )
	ROM_LOAD32_WORD( "0101046v.u8",  0x100000, 0x80000, CRC(db840d1b) SHA1(26ff790cd21f2005ae3a3e879ef07b87c8ae0020) )
	ROM_LOAD32_WORD( "0101046v.u12", 0x100002, 0x80000, CRC(eae75fa9) SHA1(576c8cf98ad4032bbdde12162e2c1bdd10056762) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100716V / 594/4 / 2 Credit Multiplier/5 Line Multiline
// Panther Magic / Local / A - 13/05/98
ROM_START( pantmaga )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000c00
	    0x000000-0x0583f7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xa310e67f
	        Calculated Checksum 0xa310e67f  (OK)
	    0x0583f8-0x1e8b5b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0583f8-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100716v.u7",  0x000000, 0x80000, CRC(8646a4a6) SHA1(1449ba497e4ccebf5de9630bfaf31ad3c583fc44) )
	ROM_LOAD32_WORD( "0100716v.u11", 0x000002, 0x80000, CRC(61ae2a5a) SHA1(3b11c0f1c6e1464211e6dea958f08d7212ee5756) )
	ROM_LOAD32_WORD( "0100716v.u8",  0x100000, 0x80000, CRC(91997f98) SHA1(2353abdc0b160cf087230f0c290f37f045a4d07b) )
	ROM_LOAD32_WORD( "0100716v.u12", 0x100002, 0x80000, CRC(6adfd0ab) SHA1(3b6479bcd95812f5678a27adc7decbc881cd6caa) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1567 / MV4115/6 / 9/20 Line Multiline Multiplier
// Party Gras / Export / A - 10/11/2001
// All devices are 27c4002 instead of 27c4096
// ROM says '9/20 Line Multiline' but game only has 9 lines
// Requires set chips 4.04.xx
// Touchscreen game
ROM_START( partygrs )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0e9b47 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x673ffb0f
	        Calculated Checksum 0x673ffb0f  (OK)
	    0x0e9b48-0x1fd2ab is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0e9b48-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1567.u7",  0x000000, 0x80000, CRC(53047385) SHA1(efe50e8785047986513f2de63d2425ba80417481) )
	ROM_LOAD32_WORD( "ahg1567.u11", 0x000002, 0x80000, CRC(f8bd9f7f) SHA1(a8c67a644f9090890e8f33e620fe0bb4633bd6e8) )
	ROM_LOAD32_WORD( "ahg1567.u8",  0x100000, 0x80000, CRC(0b98a0fa) SHA1(c9ada21e39472f28cd9b8ec19be7235410ad3e7a) )
	ROM_LOAD32_WORD( "ahg1567.u12", 0x100002, 0x80000, CRC(00d1395c) SHA1(d9a66d6cdb5aa4f583d8c23306b1416646cbde93) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )

	ROM_REGION16_BE( 0x100, "eeprom0", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom0", 0x000000, 0x000100, CRC(fea8a821) SHA1(c744cac6af7621524fc3a2b0a9a135a32b33c81b) )

	ROM_REGION16_BE( 0x100, "eeprom1", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom1", 0x000000, 0x000100, CRC(a10501f9) SHA1(34fdcd16bd7dc474baadc0836e2083abaf589549) )

	ROM_REGION( 0x80000, "nvram", 0 )
	ROM_LOAD( "nvram", 0x000000, 0x080000, CRC(fec1b1df) SHA1(5981e2961692d4c8633afea4ecb4828eabba65bd) )

	ROM_REGION( 0x20, "rtc", 0 )
	ROM_LOAD( "rtc", 0x000000, 0x00001f, CRC(6909acb0) SHA1(6a4589599cd1c477e916474e7b029e9a4e92019b) )
ROM_END


// BHG1284 / MV4115/3 / 20 Line Multiline / 3,5,10,20,25,50 Credit Multiplier
// Party Gras / Export / B - 06/02/2001
// Requires set chips 4.01.xx
// Touchscreen game
ROM_START( partygrsa )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0a69d3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xf4a004d3
	        Calculated Checksum 0xf4a004d3  (OK)
	    0x0a69d4-0x1b953f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0a69d4-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "bhg1284.u7",  0x000000, 0x80000, CRC(e9d2248a) SHA1(a010c722129cd78d14043356a24218d20d127a84) )
	ROM_LOAD32_WORD( "bhg1284.u11", 0x000002, 0x80000, CRC(d5da519b) SHA1(44b44b3839c63cde8fd6dee333b8fd70d89a5213) )
	ROM_LOAD32_WORD( "bhg1284.u8",  0x100000, 0x80000, CRC(3f72c99b) SHA1(df3a21d8c644ffa87d6c3ab091df03fa3e0ca4a4) )
	ROM_LOAD32_WORD( "bhg1284.u12", 0x100002, 0x80000, CRC(090ce72f) SHA1(a4783ca84aabd365613357a53a19a3dd48f4df0c) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1568 / MV4115/6 / 9/20 Line Multiline Multiplier
// Party Gras / Export / A - 10/11/2001
// Touchscreen game
ROM_START( partygrsb )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to ROMs being corrupted, all files are missing bytes consisting of 0x0D
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1568.u7",  0x000000, 0x7efb7, BAD_DUMP CRC(69ab6487) SHA1(d7147f78dc098d142e857687e6cbdb8a8762371a) )
	ROM_LOAD32_WORD( "ahg1568.u11", 0x000002, 0x7ff26, BAD_DUMP CRC(966bb4f8) SHA1(3fe5238acfab27c3fa6de9c00f0edf803a939df5) )
	ROM_LOAD32_WORD( "ahg1568.u8",  0x100000, 0x7f7df, BAD_DUMP CRC(175c37cb) SHA1(6047ba1ec40ad4691bd05cf12680705f841086b3) )
	ROM_LOAD32_WORD( "ahg1568.u12", 0x100002, 0x7f7da, BAD_DUMP CRC(43764888) SHA1(cfd59692f17e9ca70dc882423238f6de59dafbed) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 02J00011 / JB001 / 25 Credit Multiplier / 20 Line Multiline
// Peacock Flutter / NSW/ACT / A - 10/03/00
ROM_START( peaflut )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b98
	    0x000000-0x0638d3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x2ce2619f
	        Calculated Checksum 0x2ce2619f  (OK)
	    0x0638d4-0x1dbf8b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0638d4-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "02j00011.u7",  0x000000, 0x80000, CRC(e4497f35) SHA1(7030aba6c17fc391564385f5669e07edc94dca61) )
	ROM_LOAD32_WORD( "02j00011.u11", 0x000002, 0x80000, CRC(3134818c) SHA1(6fe158608b5da648fafd20cbcd213e6f2dc2104c) )
	ROM_LOAD32_WORD( "02j00011.u8",  0x100000, 0x80000, CRC(f239ca62) SHA1(53e3e2a4d62ceb9e921606e3670470c09e82118f) )
	ROM_LOAD32_WORD( "02j00011.u12", 0x100002, 0x80000, CRC(2d96c449) SHA1(af98a864b9ed3f95227fd0d6edc6a38c0544c93f) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200460V / 586/4(a) / 5 Credit Multiplier / 20 Line Multiline
// Penguin Pays 100cm / NSW/ACT / D - 03/06/97
ROM_START( pengpay )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x05c71f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x68980cb3
	        Calculated Checksum 0x68980cb3  (OK)
	    0x05c720-0x1aefcf is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05c720-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200460v.u7",  0x000000, 0x80000, CRC(47145744) SHA1(74a186a15537d8b05ce23f37c53f351e8058b0b2) )
	ROM_LOAD32_WORD( "0200460v.u11", 0x000002, 0x80000, CRC(82fc4e23) SHA1(54e7698c4deed7202da8f178698ecdcf85f3f640) )
	ROM_LOAD32_WORD( "0200460v.u8",  0x100000, 0x80000, CRC(8d37d7bf) SHA1(9c9b86cce9492f9de346e5a6944e2f0c5da6b9b1) )
	ROM_LOAD32_WORD( "0200460v.u12", 0x100002, 0x80000, CRC(90864742) SHA1(f6491e4fbce5d642b9d0224118923b56625338b1) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200357V / 586/4 / 5 Credit Multiplier / 20 Line Multiline
// Penguin Pays 100cm / NSW/ACT / C - 12/11/96
ROM_START( pengpaya )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b60
	    0x000000-0x05644f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x5bc8a3d6
	        Calculated Checksum 0x5bc8a3d6  (OK)
	    0x056450-0x1c19f3 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x056450-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200357v.u7",  0x000000, 0x80000, CRC(cb21de26) SHA1(5a730f08db4d91b18f0b5a1f489f1d982b08edcc) )
	ROM_LOAD32_WORD( "0200357v.u11", 0x000002, 0x80000, CRC(7dd73770) SHA1(14a2edf8cd33280464f979976486e1a9ae73cef5) )
	ROM_LOAD32_WORD( "0200357v.u8",  0x100000, 0x80000, CRC(aa95406b) SHA1(fbec024dd210757a79a5ea4def79f88fed971e71) )
	ROM_LOAD32_WORD( "0200357v.u12", 0x100002, 0x80000, CRC(123cbe90) SHA1(b51b84f79e0822a1be71485bbad514ab9fa55622) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200359V / 586/3(a) / 100 Credit Multiplier/9 Line Multiline
// Penguin Pays 90cm / NSW/ACT / D - 03/06/97
// ROM says '100 Credit Multiplier' but game has a 10 credit multiplier
ROM_START( pengpayb )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x05d7b7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xd4f9ba59
	        Calculated Checksum 0xd4f9ba59  (OK)
	    0x05d7b8-0x1c9acf is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05d7b8-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200359v.u7",  0x000000, 0x80000, CRC(f51c4e02) SHA1(fca30b3ce0d063966df1e878338596d050664695) )
	ROM_LOAD32_WORD( "0200359v.u11", 0x000002, 0x80000, CRC(c0f20ef7) SHA1(4df3aa337a2d4dd8ef29f4839b003c96fe1df526) )
	ROM_LOAD32_WORD( "0200359v.u8" , 0x100000, 0x80000, CRC(23ea514d) SHA1(bcb83a8d768b078a03260a00fa09a4e2350c568c) )
	ROM_LOAD32_WORD( "0200359v.u12", 0x100002, 0x80000, CRC(d2882682) SHA1(dd42edca8ef9d28dd5b16fe8132f8e0fb3c85979) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200113V / 586 / 20 Credit Multiplier/9 Line Multiline
// Penguin Pays / Local / A - 12/10/95
ROM_START( pengpayc )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000adc
	    0x000000-0x0537d7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x57c705e1
	        Calculated Checksum 0x57c705e1  (OK)
	    0x0537d8-0x1e2167 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0537d8-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200113v.u7",  0x000000, 0x80000, CRC(1b96bee2) SHA1(7e7ecd708236ab0765476d1dcf3aae302a676e73) )
	ROM_LOAD32_WORD( "0200113v.u11", 0x000002, 0x80000, CRC(879ddd2e) SHA1(9fc7fd18266d5b47349e6f36b6097b9f67f1da84) )
	ROM_LOAD32_WORD( "0200113v.u8",  0x100000, 0x80000, CRC(bb113f55) SHA1(48d3bb426a5a76966bc14bd5909cdc5946203f5d) )
	ROM_LOAD32_WORD( "0200113v.u12", 0x100002, 0x80000, CRC(a1865467) SHA1(572558bab639145c8260884a95646fc424687b47) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0300113V / 586 / 20 Credit Multiplier/9 Line Multiline
// Penguin Pays 180cm / NSW/ACT / A - 12/10/95
ROM_START( pengpayd )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to ROMs being corrupted, all files are missing bytes consisting of 0x0D
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0300113v.u7",  0x000000, 0x7f909, BAD_DUMP CRC(30c6c635) SHA1(9a31f99c8a7fb0a909b101b2c5767f39930934e9) )
	ROM_LOAD32_WORD( "0300113v.u11", 0x000002, 0x7ff9b, BAD_DUMP CRC(7290c743) SHA1(5f7fa8d8a0fd0bb18f2d4c81f21f39fabc34681f) )
	ROM_LOAD32_WORD( "0300113v.u8",  0x100000, 0x7faf6, BAD_DUMP CRC(1f983881) SHA1(9cefafa6074fc0f5817df226fbf01a8fb7cbbadb) )
	ROM_LOAD32_WORD( "0300113v.u12", 0x100002, 0x7fb27, BAD_DUMP CRC(7206dc37) SHA1(4d2f8551daeb4be13e73e3123e158dc1e1e4e067) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// BHI0417-03 / 586/7(b) / 10 Credit Multiplier / 9 Line Multiline
// Penguin Pays / Export / B - 14/07/97
// All devices are 27c4002 instead of 27c4096
// marked as 92.130%
// No set chips required
ROM_START( pengpayu )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0cd21b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x7dc52ffa
	        Calculated Checksum 0x7dc52ffa  (OK)
	    0x0cd21c-0x192ed7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0cd21c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "bhi041703.u7",  0x000000, 0x80000, CRC(19d75260) SHA1(798472b1b5d8f5ca99d8bfe57e99a76686f0aa3f) )
	ROM_LOAD32_WORD( "bhi041703.u11", 0x000002, 0x80000, CRC(2b010813) SHA1(a383997308881a3ac35de56fe10e3852fa89fdf6) )
	ROM_LOAD32_WORD( "bhi041703.u8",  0x100000, 0x80000, CRC(6aeaebc8) SHA1(6f70b14e9f4e9940512bd6e89bc9ccbfe1f4a81f) )
	ROM_LOAD32_WORD( "bhi041703.u12", 0x100002, 0x80000, CRC(d959a048) SHA1(92f69090d599f95b48e79213e5b7d486e083d8f4) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )

	ROM_REGION16_BE( 0x100, "eeprom0", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom0", 0x000000, 0x000100, CRC(fea8a821) SHA1(c744cac6af7621524fc3a2b0a9a135a32b33c81b) )

	ROM_REGION16_BE( 0x100, "eeprom1", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom1", 0x000000, 0x000100, CRC(8421e7c2) SHA1(fc1b07d5b7aadafc4a0f2e4dfa698e7c72340717) )

	ROM_REGION( 0x80000, "nvram", 0 )
	ROM_LOAD( "nvram", 0x000000, 0x080000, CRC(4e5b9702) SHA1(b2b645db80c4ece24fae8ce6fb660e77ac8e5810) )

	ROM_REGION( 0x20, "rtc", 0 )
	ROM_LOAD( "rtc", 0x000000, 0x00001f, CRC(6909acb0) SHA1(6a4589599cd1c477e916474e7b029e9a4e92019b) )
ROM_END


// OHG0417-03 / 586/7(b) / 10 Credit Multiplier / 9 Line Multiline
// Penguin Pays / Export / B - 14/07/97
// All devices are 27c4002 instead of 27c4096
// marked as 92.130%
// No set chips required
ROM_START( pengpayua )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0a898f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x93affe65
	        Calculated Checksum 0x93affe65  (OK)
	    0x0a8990-0x1674cf is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0a8990-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ohg041703.u7",  0x000000, 0x80000, CRC(48689db1) SHA1(6aa07a0e6dbf98a34cca31d5983ae9429f2254aa) )
	ROM_LOAD32_WORD( "ohg041703.u11", 0x000002, 0x80000, CRC(50e00a35) SHA1(e8f104621c8fc21d8ba33157729f394554c776e2) )
	ROM_LOAD32_WORD( "ohg041703.u8",  0x100000, 0x80000, CRC(a08acbb3) SHA1(8a7cc927a81521b069873a720d1a083f164dc2ef) )
	ROM_LOAD32_WORD( "ohg041703.u12", 0x100002, 0x80000, CRC(9eae7f7b) SHA1(23bd952de1d6d5d69e5565cc2bc241c2cd775453) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// OHG0417-02 / 586/7(b) / 10 Credit Multiplier / 9 Line Multiline
// Penguin Pays / Export / B - 14/07/97
// All devices are 27c4002 instead of 27c4096
// marked as 90.45%
// No set chips required
ROM_START( pengpayub )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0a898f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x96aefc60
	        Calculated Checksum 0x96aefc60  (OK)
	    0x0a8990-0x1674cf is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0a8990-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ohg041702.u7",  0x000000, 0x80000, CRC(76b7641f) SHA1(8df2c46ec8e5ec5574f12878619593e7157df517) )
	ROM_LOAD32_WORD( "ohg041702.u11", 0x000002, 0x80000, CRC(6d731b81) SHA1(06ea80e1003ceba8a790a035c520aaca99db4d54) )
	ROM_LOAD32_WORD( "ohg041702.u8",  0x100000, 0x80000, CRC(a08acbb3) SHA1(8a7cc927a81521b069873a720d1a083f164dc2ef) )
	ROM_LOAD32_WORD( "ohg041702.u12", 0x100002, 0x80000, CRC(9eae7f7b) SHA1(23bd952de1d6d5d69e5565cc2bc241c2cd775453) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1544 / MV4122 / 3,5,10,20,25,50 Credit Multiplier / 9 Line Multiline
// PENGUIN PAYS / CASH EXPRESS / Export / C - 19/01/01
// Marked SPC.
// All eproms are 27C4096.
// Cash Express Hyperlink game, but can also run standalone without progressive jackpot
// Requires set chips 4.04.xx
ROM_START( pengpayce )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0f368f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x029d0237
	        Calculated Checksum 0x029d0237  (OK)
	    0x0f3690-0x39746f is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1544.u7",  0x000000, 0x80000, CRC(c899afcf) SHA1(85b3b9f94965c2bf10b5d5b265aef28bda01a3cb) )  // game
	ROM_LOAD32_WORD( "ahg1544.u11", 0x000002, 0x80000, CRC(c44eb9bd) SHA1(6eca6459806aa7d0ab0d53e8e3e6f3c0c71f7220) )  // game
	ROM_LOAD32_WORD( "ahg1544.u8",  0x100000, 0x80000, CRC(c9ebdf24) SHA1(cf29c42d4f15edfe5ba24be6282b3a6d01a29a13) )  // base
	ROM_LOAD32_WORD( "ahg1544.u12", 0x100002, 0x80000, CRC(10c4cd0f) SHA1(db2df4d143d4ad3312d2c577e6ccbe36247f24c7) )  // base
	ROM_LOAD32_WORD( "ahg1544.u9",  0x200000, 0x80000, CRC(4eeaf222) SHA1(bb918326bee71d5597b74c6c50cbcfa3ad1ef0c4) )  // base
	ROM_LOAD32_WORD( "ahg1544.u13", 0x200002, 0x80000, CRC(5ebd90a7) SHA1(766291b62ed4a65ba6fb2d64e52022dd4831957d) )  // base
	ROM_LOAD32_WORD( "ahg1544.u10", 0x300000, 0x80000, CRC(cbbbb21f) SHA1(c9c213ac720cb7a95179db592dcadbb466ffd572) )  // base
	ROM_LOAD32_WORD( "ahg1544.u14", 0x300002, 0x80000, CRC(4ef4f063) SHA1(d6e9928f80f19d3fdd0e7acb0bfb6f9b2ae5724b) )  // base

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// EHG1257 / MV4122/1 / 3,5,10,20,25,50 Credit Multiplier / 20 Line Multiline
// PENGUIN PAYS / PENGUIN PUCKS / Export / C - 19/01/01
// Penguin Pucks Hyperlink game, but can also run standalone without progressive jackpot
// Requires set chips 4.03.xx
ROM_START( pengpuck )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0f29ef is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x6845d74f
	        Calculated Checksum 0x6845d74f  (OK)
	    0x0f29f0-0x3c1373 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ehg1257.u7",  0x000000, 0x80000, CRC(06872381) SHA1(09727389ed05819a9aefaeba7d12ec86399f0008) )
	ROM_LOAD32_WORD( "ehg1257.u11", 0x000002, 0x80000, CRC(39edca69) SHA1(03410f5f392404cd5ad72a45995ccd9bfbc2fee3) )
	ROM_LOAD32_WORD( "ehg1257.u8",  0x100000, 0x80000, CRC(06f6430f) SHA1(ea9dd4d12e573182d48d1be76110b1df12167d83) )
	ROM_LOAD32_WORD( "ehg1257.u12", 0x100002, 0x80000, CRC(623d35f2) SHA1(eaca70fc0ef91d536a570c3e7c508af4edaabe17) )
	ROM_LOAD32_WORD( "ehg1257.u9",  0x200000, 0x80000, CRC(0e617716) SHA1(7abcf0010d5a9f0103c123b11398416bc8dc8529) )
	ROM_LOAD32_WORD( "ehg1257.u13", 0x200002, 0x80000, CRC(b35e690a) SHA1(90616892169cbb24abad35f22000ab10ae94331a) )
	ROM_LOAD32_WORD( "ehg1257.u10", 0x300000, 0x80000, CRC(8afb5df5) SHA1(2654034776160abe7fb0169c3e773204ea90acf6) )
	ROM_LOAD32_WORD( "ehg1257.u14", 0x300002, 0x80000, CRC(b6cb5809) SHA1(84288a41d2a3980bf68e9a32b9402652ac6a16d6) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100674V / 619/1 / 10 Credit Multiplier/20 Line Multiline
// Penguin Pirate 200cm / NSW/ACT / A - 31/03/98
ROM_START( penpir )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x05d27b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xd3b95e9b
	        Calculated Checksum 0xd3b95e9b  (OK)
	    0x05d27c-0x1ce66f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05d27c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100674v.u7",  0x000000, 0x80000, CRC(219113d9) SHA1(76aaa3aff0ecb86b749271e0aa4670738c6f263d) )
	ROM_LOAD32_WORD( "0100674v.u11", 0x000002, 0x80000, CRC(91c57c7b) SHA1(95128ba87d35be2a489b33e381dee3be573b4a53) )
	ROM_LOAD32_WORD( "0100674v.u8",  0x100000, 0x80000, CRC(8c738184) SHA1(46385dc6e848f065f0b840bb13709bae90ab9b9f) )
	ROM_LOAD32_WORD( "0100674v.u12", 0x100002, 0x80000, CRC(acdbbbe6) SHA1(5909135163af18c3ecd84934612e3751ca62ae74) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200578V / 619 / 10 Credit Multiplier/9 Line Multiline
// Penguin Pirate 90cm / NSW/ACT / C - 27/02/98
ROM_START( penpira )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x05feeb is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x2485f5b8
	        Calculated Checksum 0x2485f5b8  (OK)
	    0x05feec-0x1cdcc7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05feec-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200578v.u7",  0x000000, 0x80000, CRC(4bdb20a2) SHA1(f7121a65556b13b58eca7a8816c7d49a10233171) )
	ROM_LOAD32_WORD( "0200578v.u11", 0x000002, 0x80000, CRC(45a4c6ff) SHA1(7692d4bdeca75bb5c7cc89ada267bd58c620d774) )
	ROM_LOAD32_WORD( "0200578v.u8",  0x100000, 0x80000, CRC(8b105815) SHA1(b77134d73bc61c2c3995f18991cbf1473a9f6293) )
	ROM_LOAD32_WORD( "0200578v.u12", 0x100002, 0x80000, CRC(7ea30a52) SHA1(70d7640ea237197be4b2b8c64c2a97ad7b608cc0) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100869V / 619/3 / 10 Credit Multiplier/20 Line Multiline
// Penguin Pirate 2 / Crown / A - 17/12/98
// ROM says 'Penguin Pirate 2', artwork says 'Penguin Pirate II'
// Original casino BIOS is not dumped, using New Zealand 0700474V BIOS until an Australian version is dumped
ROM_START( penpir2 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    note, this actually contains a 2nd checksum for the game, this is the base/bios check only.

	    Checksum code found at 0x001b74
	    0x000000-0x089a2f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x5ad8a58b
	        Calculated Checksum 0x5ad8a58b  (OK)
	    0x089a30-0x2a8ecf is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x089a30-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	// these are the 'bios' for Casino games (could be moved to a different base set)
	ROM_LOAD32_WORD( "0700474v.u7",  0x000000, 0x80000, CRC(04b7dcbf) SHA1(eded1223336181bb08f9593247f1f79d96278b75) )
	ROM_LOAD32_WORD( "0700474v.u11", 0x000002, 0x80000, CRC(a89ce1b5) SHA1(411b474a111f23ebd834bea5af0bf0cf3926d590) )

	ROM_LOAD32_WORD( "0100869v.u8",  0x100000, 0x80000, CRC(5a87f637) SHA1(0fee8140637e9f923727e8c358c9f59b8319855d) )
	ROM_LOAD32_WORD( "0100869v.u12", 0x100002, 0x80000, CRC(2aef04c1) SHA1(7415f436960c7b4a43634161ca317b2ae34ee745) )
	ROM_LOAD32_WORD( "0100869v.u9",  0x200000, 0x80000, CRC(05de2653) SHA1(7d3f9d50013d8137cef285940b04209cfdae4a1d) )
	ROM_LOAD32_WORD( "0100869v.u13", 0x200002, 0x80000, CRC(e1dbfd58) SHA1(687b7254279734e1835e1713d032b5aa2cf70812) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100731V / 618/1 / 10 Credit Multiplier / 20 Line Multiline
// Pet Shop / Local / A - 17/04/98
ROM_START( petshop )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b88
	    0x000000-0x05f127 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x4ea85490
	        Calculated Checksum 0x4ea85490  (OK)
	    0x05f128-0x195c0b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05f128-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100731v.u7",  0x000000, 0x80000, CRC(01cffccc) SHA1(a39d943e700fff34d82bcff8c61f2586ee65e673) )
	ROM_LOAD32_WORD( "0100731v.u11", 0x000002, 0x80000, CRC(a8e906c5) SHA1(f6dd7bcf5fa90933c9741699f0c1e07b685ccb40) )
	ROM_LOAD32_WORD( "0100731v.u8",  0x100000, 0x80000, CRC(757e1296) SHA1(e14508bbaa3439a93c8b716267a2198ed3c54728) )
	ROM_LOAD32_WORD( "0100731v.u12", 0x100002, 0x80000, CRC(6e74cd57) SHA1(9092e656cbd8627b208b81ca0d737483a779bce1) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END

// 0100679V / 618 / 10 Credit Multiplier / 9 Line Multiline
// Pet Shop / Local / A - 09/03/98
ROM_START( petshopa )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to bad ROMs, all 4 ROMs have stuck bits (0x20)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100679v.u7",  0x000000, 0x80000, BAD_DUMP CRC(cf4a24fa) SHA1(b510de9199d16ba7319e1b692d7c6c09fcb735dc) )
	ROM_LOAD32_WORD( "0100679v.u11", 0x000002, 0x7fffd, BAD_DUMP CRC(bfaa9216) SHA1(19f1c7de05ff7f5f9f370be00cf8f0635e966809) ) // wrong size!
	ROM_LOAD32_WORD( "0100679v.u8",  0x100000, 0x80000, BAD_DUMP CRC(bb9f7519) SHA1(fa311f1ec74c3b52e2feed36d7b7dc6a12336abe) )
	ROM_LOAD32_WORD( "0100679v.u12", 0x100002, 0x80000, BAD_DUMP CRC(2cd12986) SHA1(b6b0bd6dd8c964498edc3763cb5c450795042a8d) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0500005V / 570/1 / 10 Credit Multiplier/9 Line Multiline
// Phantom Pays / Local / E - 12/09/95
ROM_START( phantpay )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000adc
	    0x000000-0x044713 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x5e398313
	        Calculated Checksum 0x5e398313  (OK)
	    0x044714-0x1d8f87 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x044714-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0500005v.u7",  0x000000, 0x80000, CRC(2cfc44a7) SHA1(a2a93047311d7a1f45e2915478ba2a11d5179194) )
	ROM_LOAD32_WORD( "0500005v.u11", 0x000002, 0x80000, CRC(3e91ed2a) SHA1(92d49bd78d329ad53cb2063af2d324eada3f53d1) )
	ROM_LOAD32_WORD( "0500005v.u8",  0x100000, 0x80000, CRC(ab1e77e9) SHA1(5a8da1210214ccc89dfde2e28f5142036a743172) )
	ROM_LOAD32_WORD( "0500005v.u12", 0x100002, 0x80000, CRC(d43a092a) SHA1(5f851bd179b14ef3983b460ed932810f3713d3e5) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100299V / 578/4 / 3 Credit Multiplier/3 Line Multiline
// Prize Fight / Local / B - 08/08/96
ROM_START( przfight )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b48
	    0x000000-0x053def is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x97c4e600
	        Calculated Checksum 0x97c4e600  (OK)
	    0x053df0-0x2a9f7f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x053df0-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100299v.u7",  0x000000, 0x80000, CRC(2b1a9678) SHA1(c75de4c76cd934df746040d0515694d92e2fc145) )
	ROM_LOAD32_WORD( "0100299v.u11", 0x000002, 0x80000, CRC(e1bf20d7) SHA1(bcc308b884433b3ebd890fafa667235a9fb7876c) )
	ROM_LOAD32_WORD( "0100299v.u8",  0x100000, 0x80000, CRC(92b68d43) SHA1(74ba55d6c7016de26692138d194f57f016feb938) )
	ROM_LOAD32_WORD( "0100299v.u12", 0x100002, 0x80000, CRC(b4797555) SHA1(695aa6c40145fd9856821288680a24d316b7d4cd) )
	ROM_LOAD32_WORD( "0100299v.u9",  0x200000, 0x80000, CRC(b3163b0c) SHA1(e9aac4acb31a9af194626b25517aa7c169fe40bf) )
	ROM_LOAD32_WORD( "0100299v.u13", 0x200002, 0x80000, CRC(c16197d5) SHA1(716c4afdf2acde10ff09ad90b03bc5e689f0a737) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100706V / 603/6 / 10 Credit Multiplier / 20 Line Multiline
// Queens of Cash / NSW/ACT / C  - 23/07/98
ROM_START( qcash )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000af4
	    0x000000-0x05d55b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x10b06e83
	        Calculated Checksum 0x10b06e83  (OK)
	    0x05d55c-0x1a669f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05d55c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100706v.u7",  0x000000, 0x80000, CRC(591c96eb) SHA1(acd6f02206086d710a92401c618f715b3646d78a) )
	ROM_LOAD32_WORD( "0100706v.u11", 0x000002, 0x80000, CRC(5001567e) SHA1(eadde9750856a7920e06955adc0db46082da655a) )
	ROM_LOAD32_WORD( "0100706v.u8",  0x100000, 0x80000, CRC(31ed5795) SHA1(8238da7c87195339d34cf24b3e0a7f3bf53d2b8a) )
	ROM_LOAD32_WORD( "0100706v.u12", 0x100002, 0x80000, CRC(bfedb3fc) SHA1(e115db94b8ee7babb29e31e64b96d181f5c6491b) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0300439V / 602/4 / 25 Credit Multiplier / 20 Line Multiline
// QUEEN OF THE NILE / NSW/ACT / B - 13/05/97
// EPROM labels have "Golden Pyramid 500" (no trailing 's') and 2001 for the year
ROM_START( qnile )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b80
	    0x000000-0x062913 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x2d52d80f
	        Calculated Checksum 0x2d52d80f  (OK)
	    0x062914-0x1740eb is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x062914-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0300439v.u7",  0x000000, 0x80000, CRC(63f9129e) SHA1(a513fd47d3ca4fe007730a06e5f6ffc2891dc74f) )
	ROM_LOAD32_WORD( "0300439v.u11", 0x000002, 0x80000, CRC(7217c3af) SHA1(518c3d79758e3253f937cf73da9398fa812bf4bc) )
	ROM_LOAD32_WORD( "0300439v.u8",  0x100000, 0x80000, CRC(90c92bf8) SHA1(bbc558ffb5a883c9f4ff9dc3362c4081990c970d) )
	ROM_LOAD32_WORD( "0300439v.u12", 0x100002, 0x80000, CRC(eec01bb4) SHA1(146fdce6b32a21659dc775e4a5f3bb027bd09825) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200439V / 602/4 / 25 Credit Multiplier / 20 Line Multiline
// QUEEN OF THE NILE / NSW/ACT / B - 13/05/97
// Same strings as qnile but earlier version
ROM_START( qnilea )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b80
	    0x000000-0x059dff is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xa63a9b3e
	        Calculated Checksum 0xa63a9b3e  (OK)
	    0x059e00-0x16b5d7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x059e00-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200439v.u7",  0x000000, 0x80000, CRC(d476a893) SHA1(186d6fb1830c33976f2d3c96e4f045ece885dc63) )
	ROM_LOAD32_WORD( "0200439v.u11", 0x000002, 0x80000, CRC(8b0d7205) SHA1(ffa03f1c9332a1a7443eb91b0ded56e7cd9e3cee) )
	ROM_LOAD32_WORD( "0200439v.u8",  0x100000, 0x80000, CRC(9b996ef1) SHA1(72489e9a0ee5c34f7cad3d121bcd08e09ef72360) )
	ROM_LOAD32_WORD( "0200439v.u12", 0x100002, 0x80000, CRC(2a0f7feb) SHA1(27c89dadf759e6c892121650758c44ec50990cb6) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100439V / 602/4 / 25 Credit Multiplier / 20 Line Multiline
// QUEEN OF THE NILE / NSW/ACT / B - 13/05/97
// Same strings as qnile and qnilea but earlier version
ROM_START( qnileb )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b80
	    0x000000-0x055c83 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x53fa5304
	        Calculated Checksum 0x53fa5304  (OK)
	    0x055c84-0x16745b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x055c84-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100439v.u7",  0x000000, 0x80000, CRC(f359afcf) SHA1(a8cbaea899f0108a179c58ec97241a57227afa79) )
	ROM_LOAD32_WORD( "0100439v.u11", 0x000002, 0x80000, CRC(ca4fe491) SHA1(2bd799f95c9a5afb7c96305bf56413ba864a26dd) )
	ROM_LOAD32_WORD( "0100439v.u8",  0x100000, 0x80000, CRC(80efde3a) SHA1(1fac1b150c5c8c52a4caaa01c4571a0e7033278d) )
	ROM_LOAD32_WORD( "0100439v.u12", 0x100002, 0x80000, CRC(bdcec4eb) SHA1(ef3658460263cd2e68e10015efdc016ad705213e) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101707V / MV4162 / 10 Credit Multiplier / 9 Line Multiline
// Queen of the Nile / South America - Brazil / A - 21/08/02
// Game has lotto symbols in place of 9 through Ace, the lotto symbols are the same colour as the symbols they replaced
// 3 = Nine, 4 = Ten, 5 = Jack, 6 = Queen, 7 = King, 8 = Ace
// Game and BIOS are in Portuguese
ROM_START( qnilebr )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    note, this actually contains a 2nd checksum for the game, this is the base/bios check only.

	    Checksum code found at 0x001b74
	    0x000000-0x0d1c93 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x0389c2a1
	        Calculated Checksum 0x0389c2a1  (OK)
	    0x0d1c94-0x23692f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0d1c94-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	// these are the 'bios' for Casino games (could be moved to a different base set)
	ROM_LOAD32_WORD( "0301718v.u7",  0x000000, 0x80000, CRC(e0bf299d) SHA1(9015c912b8dd652f07a80baa4c0776b44dc60f65) )
	ROM_LOAD32_WORD( "0301718v.u11", 0x000002, 0x80000, CRC(7696ab70) SHA1(65ae908ff7cc67334d866afe6fcc1e81fac1d962) )

	ROM_LOAD32_WORD( "0101707v.u8",  0x100000, 0x80000, CRC(3d4707ca) SHA1(75d037784046f2ff660fc427285dc0964a98c56b) )
	ROM_LOAD32_WORD( "0101707v.u12", 0x100002, 0x80000, CRC(f80d4b86) SHA1(b798d93266d93cde5299abd30689812df52f03ab) )
	ROM_LOAD32_WORD( "0101707v.u9",  0x200000, 0x80000, CRC(0d688398) SHA1(f1f8c269f52d196dda0946406d85f8c63c990c64) )
	ROM_LOAD32_WORD( "0101707v.u13", 0x200002, 0x80000, CRC(1b5da8ca) SHA1(9a6cfbadba24677a0d82a61ffdc25772ecbac287) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0300440V / 602/3 / 20 Credit Multiplier / 9 Line Multiline
// QUEEN OF THE NILE / NSW/ACT / B - 13/05/97
ROM_START( qnilec )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b80
	    0x000000-0x064c4b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xa60cbcfa
	        Calculated Checksum 0xa60cbcfa  (OK)
	    0x064c4c-0x172a17 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x064c4c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0300440v.u7",  0x000000, 0x80000, CRC(0076da68) SHA1(ed301c102e88d5b637144ed32042da46780e5b34) )
	ROM_LOAD32_WORD( "0300440v.u11", 0x000002, 0x80000, CRC(b5b76fb0) SHA1(40cb57e168f7884d64f6779e4e3b532c69df63b8) )
	ROM_LOAD32_WORD( "0300440v.u8",  0x100000, 0x80000, CRC(a6b856a2) SHA1(2a9ea01f64fa56dea86b0cd25e19dace34c17d0f) )
	ROM_LOAD32_WORD( "0300440v.u12", 0x100002, 0x80000, CRC(52bd3694) SHA1(bcfa3054c7577f7a1653b756828d048a5f1776e7) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1609 / MV4091/1 / 5,10,25,50 Credit Multiplier / 20 Line Multiline
// QUEEN OF THE NILE / Export / A - 17/01/01
// Cash Express Hyperlink game, but can also run standalone without progressive jackpot
// Requires set chips 4.04.xx
ROM_START( qnilece )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0f2453 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xb51d7d67
	        Calculated Checksum 0xb51d7d67  (OK)
	    0x0f2454-0x3bcd33 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1609.u7",  0x000000, 0x80000, CRC(4596f2dc) SHA1(62ca6ec2c22a79feaeddc43697589e3ed2672367) )
	ROM_LOAD32_WORD( "ahg1609.u11", 0x000002, 0x80000, CRC(75ec9cfb) SHA1(5a8dfed93774fbf255b060b7b212e699348b364d) )
	ROM_LOAD32_WORD( "ahg1609.u8",  0x100000, 0x80000, CRC(e7ee132e) SHA1(9360ca77e1a2ffa7c0fecf74b949032f887a21e7) )
	ROM_LOAD32_WORD( "ahg1609.u12", 0x100002, 0x80000, CRC(23ed5c0d) SHA1(ad35c9de62c3c76dabc6b7a78a25d1f2cd9cb9ac) )
	ROM_LOAD32_WORD( "ahg1609.u9",  0x200000, 0x80000, CRC(0fc7c457) SHA1(792c212371e8a141eaf7f0c26e45905d765ff941) )
	ROM_LOAD32_WORD( "ahg1609.u13", 0x200002, 0x80000, CRC(79e4fc75) SHA1(621f78e4c65acd49643b9fd955f070061ff38050) )
	ROM_LOAD32_WORD( "ahg1609.u10", 0x300000, 0x80000, CRC(d565f21c) SHA1(0dd858c61338f9d6a99d787f813a4b4a4e553fb2) )
	ROM_LOAD32_WORD( "ahg1609.u14", 0x300002, 0x80000, CRC(fe7817f3) SHA1(a3579ed7ce6d999b0f71482f2a2e3ade693668fb) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1525 / MV4091 / 10 Credit Multiplier / 20 Line Multiline
// QUEEN OF THE NILE / Export / F - 17/01/01
// ROM says '10 Credit Multiplier / 20 Line Multiline' but it is a 9 line game with 3,5,10,20,25,50 credit multipliers
// Cash Express Hyperlink game, but can also run standalone without progressive jackpot
// Requires set chips 4.04.xx
ROM_START( qnilecea )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0ee84f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x34434fc2
	        Calculated Checksum 0x34434fc2  (OK)
	    0x0ee850-0x3b9137 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1525.u7",  0x000000, 0x80000, CRC(0c7f5a0d) SHA1(71e5f0b0dfd8a9176d18ab0144161e0b32aa4d0e) )
	ROM_LOAD32_WORD( "ahg1525.u11", 0x000002, 0x80000, CRC(24b1e4f6) SHA1(0c6532b296cfa44f94b893483042cb669b023829) )
	ROM_LOAD32_WORD( "ahg1525.u8",  0x100000, 0x80000, CRC(3c5e5edd) SHA1(46cd9609ace5dedf6bda1892b7d7926c4c1abc21) )
	ROM_LOAD32_WORD( "ahg1525.u12", 0x100002, 0x80000, CRC(e4596d6b) SHA1(cf433e77947e172538ba245cf19f00dd97594c07) )
	ROM_LOAD32_WORD( "ahg1525.u9",  0x200000, 0x80000, CRC(98d6e2d3) SHA1(3e8718b7f2a50e437fc231601c27fed8373ddaf2) )
	ROM_LOAD32_WORD( "ahg1525.u13", 0x200002, 0x80000, CRC(73138015) SHA1(86202a24f3ebcecb3265f625fe83150d197781aa) )
	ROM_LOAD32_WORD( "ahg1525.u10", 0x300000, 0x80000, CRC(2005e638) SHA1(69b56885294d84cbc12e4a98f9818aa3efc44ab5) )
	ROM_LOAD32_WORD( "ahg1525.u14", 0x300002, 0x80000, CRC(520452d4) SHA1(6d501b82a7b35a6e53a0991cbc384b752a295edf) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1608 / MV4091 / 5,10,20 Credit Multiplier / 9 Line Multiline
// QUEEN OF THE NILE / Export / F - 17/01/01
// Marked as SPC.
// All eproms are 27C4096.
// Cash Express Hyperlink game, but can also run standalone without progressive jackpot
// Requires set chips 4.04.xx
ROM_START( qnileceb )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0f237f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xad17f6d5
	        Calculated Checksum 0xad17f6d5  (OK)
	    0x0f2380-0x3bcc5f is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1608.u7",  0x000000, 0x80000, CRC(220d4dbc) SHA1(89dc37c6774b7e066868130ce2985a0d86953ec4) )
	ROM_LOAD32_WORD( "ahg1608.u11", 0x000002, 0x80000, CRC(f11e9f7b) SHA1(3d282dd779f48bca98c4d4ebca4e7206e0f2dff6) )
	ROM_LOAD32_WORD( "ahg1608.u8",  0x100000, 0x80000, CRC(7101c670) SHA1(51cca228ca219c778761e500ac37953e874e76f5) )
	ROM_LOAD32_WORD( "ahg1608.u12", 0x100002, 0x80000, CRC(a7875dbf) SHA1(4944773ec12aba122f474975fa52ca28365123df) )
	ROM_LOAD32_WORD( "ahg1608.u9",  0x200000, 0x80000, CRC(33a4099e) SHA1(a774742332dfb11c9396e08a9a48f9274e76a58a) )
	ROM_LOAD32_WORD( "ahg1608.u13", 0x200002, 0x80000, CRC(221c24af) SHA1(218bc944a9ba2d89c9b9b8d29e61a37048e70adc) )
	ROM_LOAD32_WORD( "ahg1608.u10", 0x300000, 0x80000, CRC(49e35226) SHA1(6d3396ff74dd892a009edf32dc3a7677979a9854) )
	ROM_LOAD32_WORD( "ahg1608.u14", 0x300002, 0x80000, CRC(4d0f1d12) SHA1(980775e628e9508398cb5227af10ed9ff0555b8a) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101139V / 602/16 / 3 Credit Multiplier / 3 Line Multiline
// QUEEN OF THE NILE / NSW/ACT / A - 11/10/99
ROM_START( qniled )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b78
	    0x000000-0x068183 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xae799f3b
	        Calculated Checksum 0xae799f3b  (OK)
	    0x068184-0x251633 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x068184-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101139v.u7",  0x000000, 0x80000, CRC(07ee2925) SHA1(b73e5124986020202e06dd907fbdbfb5f9ad2141) )
	ROM_LOAD32_WORD( "0101139v.u11", 0x000002, 0x80000, CRC(bcbdd3f8) SHA1(f8429a4077f9ae6f7e7b4f9a0feed6f5ec9b8126) )
	ROM_LOAD32_WORD( "0101139v.u8",  0x100000, 0x80000, CRC(02e06b31) SHA1(23316ae3d0f5907c4e32796c45519089ec5c1622) )
	ROM_LOAD32_WORD( "0101139v.u12", 0x100002, 0x80000, CRC(f2d6238f) SHA1(0aa847b664d7a322a845ce8d941b0afab6765d7d) )
	ROM_LOAD32_WORD( "0101139v.u9",  0x200000, 0x80000, CRC(37a0534c) SHA1(8db2184ee93e8879234c865b4464b5994e96a10a) )
	ROM_LOAD32_WORD( "0101139v.u13", 0x200002, 0x80000, CRC(160fbbb5) SHA1(fe2ce9b915b0dfcbc60deed30a95550a21a18127) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101670V / 602/4 / 25, 50 Credit / 20 Line Multiline
// Queen Of The Nile - Fast Lane / Qld QCOM / E - 17/08/01
// Fast Lane Hyperlink game
ROM_START( qnilefl )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ad0
	    0x000000-0x086cbb is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x3297c26b
	        Calculated Checksum 0x3297c26b  (OK)
	    0x086cbc-0x2aecd3 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101670v.u7",  0x000000, 0x80000, CRC(71817007) SHA1(90cd928894044b629e03aa2c1d533eb6aed4c7af) )
	ROM_LOAD32_WORD( "0101670v.u11", 0x000002, 0x80000, CRC(53006d14) SHA1(00d99e74433023d007f6a401d0081decb36a32dc) )
	ROM_LOAD32_WORD( "0101670v.u8",  0x100000, 0x80000, CRC(ffd13e39) SHA1(d0fd0efc968fc4c8c81b0824eb9773462b88acb4) )
	ROM_LOAD32_WORD( "0101670v.u12", 0x100002, 0x80000, CRC(d918d776) SHA1(aa3729d712a43b777d648c840b3e3de21d76ab50) )
	ROM_LOAD32_WORD( "0101670v.u9",  0x200000, 0x80000, CRC(d8c46bd8) SHA1(ae9427f668fe109573915ca0408450137f75ade0) )
	ROM_LOAD32_WORD( "0101670v.u13", 0x200002, 0x80000, CRC(715e7789) SHA1(b734aa88e7e65fba988e7fffd711039c4f554b2c) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0401072V / 602/4 / 25 Credit Multiplier / 20 Line Multiline
// QUEEN OF THE NILE / NSW/ACT / D - 18/06/99
// Maximillions Hyperlink game
ROM_START( qnilemax )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000bb8
	    0x000000-0x06fd6f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xcd901653
	        Calculated Checksum 0xcd901653  (OK)
	    0x06fd70-0x3864c7 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0401072v.u7",  0x000000, 0x80000, CRC(4ac2a82e) SHA1(3fc50e97ad48c57e21a37fbb6142152c72055ad4) )
	ROM_LOAD32_WORD( "0401072v.u11", 0x000002, 0x80000, CRC(6ae3872f) SHA1(f8c1b31e4ebd4833dcc2b7cfff25f6473ad78f4e) )
	ROM_LOAD32_WORD( "0401072v.u8",  0x100000, 0x80000, CRC(abbbf1de) SHA1(5efd88213180846ad8347e017e5ccee5b80b95d0) )
	ROM_LOAD32_WORD( "0401072v.u12", 0x100002, 0x80000, CRC(65cea496) SHA1(4b827e2707c259717bf759e76dca1c96efada926) )
	ROM_LOAD32_WORD( "0401072v.u9",  0x200000, 0x80000, CRC(750150eb) SHA1(a9ffe0b0bb2ef83a696fa568b0264d27bc650120) )
	ROM_LOAD32_WORD( "0401072v.u13", 0x200002, 0x80000, CRC(1b77bbd0) SHA1(c93d2f844032631d9594d02fa6ac41e21025a8ea) )
	ROM_LOAD32_WORD( "0401072v.u10", 0x300000, 0x80000, CRC(8491dbc4) SHA1(f0d4e470f0774a6aac168334390c116fd3d1075e) )
	ROM_LOAD32_WORD( "0401072v.u14", 0x300002, 0x80000, CRC(4fbccf72) SHA1(97d6bb400caf78cb673a324d48d2580f1bbb1acd) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0301059V / 602/5 / 10 Credit Multiplier / 9 Line Multiline
// QUEEN OF THE NILE / HOLLAND / G - 10/04/00
ROM_START( qnilenl )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x00104c
	    0x000000-0x05d1cb is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x1a708478
	        Calculated Checksum 0x1a708478  (OK)
	    0x05d1cc-0x16add3 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05d1cc-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0301059v.u7",  0x000000, 0x80000, CRC(99aa5674) SHA1(54d710b7c70c82a34c07bc699bf9c2ace7f660c3) )
	ROM_LOAD32_WORD( "0301059v.u11", 0x000002, 0x80000, CRC(c9726930) SHA1(50915edbde3672d94236b2f416c490466b5ac1c6) )
	ROM_LOAD32_WORD( "0301059v.u8",  0x100000, 0x80000, CRC(d3cd3939) SHA1(0448722e44ee5ef191c9f2abab3faf2596284822) )
	ROM_LOAD32_WORD( "0301059v.u12", 0x100002, 0x80000, CRC(e505912a) SHA1(ae71aa6d56d424383add3b9cbc17473ab0a13bdc) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0300785V / MV4068 / 5 Credit Multiplier/9 Line Multiline
// Queen of the nile / New Zealand / A- 31/8/98
// ROM has "nile" in lowercase
ROM_START( qnilenz )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found (uses different startup sequence)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0300785v.u7",  0x000000, 0x80000, CRC(c327d801) SHA1(4721d87a7a759bd437d0d93dc6c415e7a8e60ea5) )
	ROM_LOAD32_WORD( "0300785v.u11", 0x000002, 0x80000, CRC(6bf94b80) SHA1(82d5fa551818bd23eecceaa992ec0d9992c53786) )
	ROM_LOAD32_WORD( "0300785v.u8",  0x100000, 0x80000, CRC(23e5aa25) SHA1(1b77f5ad1cd990e5b21ee9fa018e7ea584cc871e) )
	ROM_LOAD32_WORD( "0300785v.u12", 0x100002, 0x80000, CRC(b938825c) SHA1(137abb5573fe120ca9cb1fa181bd8591889420d0) )
	ROM_LOAD32_WORD( "0300785v.u9",  0x200000, 0x80000, CRC(35244a36) SHA1(2529ee5d2d8419d050dc1af501adba24f8e1fbcb) )
	ROM_LOAD32_WORD( "0300785v.u13", 0x200002, 0x80000, CRC(b267d25a) SHA1(8065601190540e5f0b4a91fed1deffcdb2606f94) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0201200V / 602/4 / 25 Credit Multiplier / 20 Line Multiline
// Queen Of The Nile / Qld QCOM / D - 18/06/99
ROM_START( qnileql )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ad0
	    0x000000-0x0807ff is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x36105c3f
	        Calculated Checksum 0x36105c3f  (OK)
	    0x080800-0x1aa743 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x080800-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0201200v.u7",  0x000000, 0x80000, CRC(093b575c) SHA1(3139e1f1e94f752e2162af8064ab7d62f0b15b07) )
	ROM_LOAD32_WORD( "0201200v.u11", 0x000002, 0x80000, CRC(2a356826) SHA1(4c9ae643570fceff7138e1a7bb780de9eaa272b0) )
	ROM_LOAD32_WORD( "0201200v.u8",  0x100000, 0x80000, CRC(8b3dd4cb) SHA1(b57494001bfd6a7b933214b62f5c93cb8c21fdca) )
	ROM_LOAD32_WORD( "0201200v.u12", 0x100002, 0x80000, CRC(38332f4e) SHA1(53c90103df06e5a28bbaf32504a0984b7c65bec7) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// GHG4091-02 / MV4091 / 10 Credit Multiplier / 9 Line Multiline
// QUEEN OF THE NILE / NSW/ACT / B - 13/05/97
// All devices are 27c4002 instead of 27c4096
// ROM contains unaltered NSW/ACT region string and date, but game is for the US platform
// No set chips required
ROM_START( qnileu )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x08ec87 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xb2ac33b8
	        Calculated Checksum 0xb2ac33b8  (OK)
	    0x08ec88-0x1aca67 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x08ec88-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ghg409102.u7",  0x000000, 0x80000, CRC(a00ab2cf) SHA1(eb3120fe4b1d0554c224c7646e727e86fd35975e) )
	ROM_LOAD32_WORD( "ghg409102.u11", 0x000002, 0x80000, CRC(c4a35337) SHA1(d469ed154caed1f0a4cf89e67d852924c95172ed) )
	ROM_LOAD32_WORD( "ghg409102.u8",  0x100000, 0x80000, CRC(16a629e1) SHA1(0dee11a2f1b2068a86b3e0b6c01d115555a657c9) )
	ROM_LOAD32_WORD( "ghg409102.u12", 0x100002, 0x80000, CRC(7871a846) SHA1(ac1d741092afda842e1864f1a7a14137a9ee46d9) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )

	ROM_REGION16_BE( 0x100, "eeprom0", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom0", 0x000000, 0x000100, CRC(fea8a821) SHA1(c744cac6af7621524fc3a2b0a9a135a32b33c81b) )

	ROM_REGION16_BE( 0x100, "eeprom1", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom1", 0x000000, 0x000100, CRC(1fc27753) SHA1(7e5008faaf115dc506481430272285117c989d8e) )

	ROM_REGION( 0x80000, "nvram", 0 )
	ROM_LOAD( "nvram", 0x000000, 0x080000, CRC(5a7bb53a) SHA1(cdac900925d0ee8f98209a377b9f8760de0c2883) )

	ROM_REGION( 0x20, "rtc", 0 )
	ROM_LOAD( "rtc", 0x000000, 0x00001f, CRC(6909acb0) SHA1(6a4589599cd1c477e916474e7b029e9a4e92019b) )
ROM_END


// GHG4091-03 / MV4091 / 10 Credit Multiplier / 9 Line Multiline
// QUEEN OF THE NILE / NSW/ACT / B - 13/05/97
// Marked as 94.941%
// All devices are 27C4002.
// ROM contains unaltered NSW/ACT region string and date, but game is for the US platform
// Note: The game has GHG4090-03 in the stickers, but the strings inside are GHG4091-03 instead. Also the base is GHG4091.
// No set chips required
ROM_START( qnileua )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x08ec87 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xbaad35ad
	        Calculated Checksum 0xbaad35ad  (OK)
	    0x08ec88-0x1aca67 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x08ec88-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ghg409103.u7",  0x000000, 0x80000, CRC(63c5e917) SHA1(9b1735999d66e4cf879803980c8ac03513445589) )  // 94.941%
	ROM_LOAD32_WORD( "ghg409103.u11", 0x000002, 0x80000, CRC(0b9256a1) SHA1(711484463bfaa6c6b038a2c8cabd4dc009e55ad3) )  // 94.941%
	ROM_LOAD32_WORD( "ghg4091.u8",    0x100000, 0x80000, CRC(16a629e1) SHA1(0dee11a2f1b2068a86b3e0b6c01d115555a657c9) )  // base
	ROM_LOAD32_WORD( "ghg4091.u12",   0x100002, 0x80000, CRC(7871a846) SHA1(ac1d741092afda842e1864f1a7a14137a9ee46d9) )  // base

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 04J00784 / 602/3 / 20 Credit Multiplier / 9 Line Multiline
// QUEEN OF THE NILE / NSW/ACT / B - 13/05/97
// ROM contains unaltered NSW/ACT region string and date, but game is for the Venezuelan market
// Game is in Spanish, however audit mode is in English
ROM_START( qnilev )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b80
	    0x000000-0x081a0b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xf6f60d0b
	        Calculated Checksum 0xf6f60d0b  (OK)
	    0x081a0c-0x18f7d7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x081a0c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "04j00784.u7",  0x000000, 0x80000, CRC(e42c82a2) SHA1(1f6f9a349210ed859f47ce43958c84d59e169854) )
	ROM_LOAD32_WORD( "04j00784.u11", 0x000002, 0x80000, CRC(25923a01) SHA1(f958b2dc0155077ea5c7bd87dfd16b42fc2d8d17) )
	ROM_LOAD32_WORD( "04j00784.u8",  0x100000, 0x80000, CRC(5b2e6830) SHA1(9b21abae77d20a3be28dc5c1b0ecbb2ae9197db9) )
	ROM_LOAD32_WORD( "04j00784.u12", 0x100002, 0x80000, CRC(621adc77) SHA1(2fecef64139c502b0baee5c945fe5671532aacab) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0500009V / 581 / 5 Credit Multiplier/9 Line Multiline
// Q.T. Bird / Local / A - 27/10/94
ROM_START( qtbird )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ae0
	    0x000000-0x0454af is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x6bfd2884
	        Calculated Checksum 0x6bfd2884  (OK)
	    0x0454b0-0x1b2f8b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0454b0-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0500009v.u7",  0x000000, 0x80000, CRC(f294fc0a) SHA1(f3d60ca6008445f535fce027f5ec3fe82ae552c3) )
	ROM_LOAD32_WORD( "0500009v.u11", 0x000002, 0x80000, CRC(328b7e04) SHA1(5c49f60b7c88d6e94e7ab464fad4eee6806f327a) )
	ROM_LOAD32_WORD( "0500009v.u8",  0x100000, 0x80000, CRC(764b5568) SHA1(a097992499044b7ca017a8c85387dc1ea94ff27a) )
	ROM_LOAD32_WORD( "0500009v.u12", 0x100002, 0x80000, CRC(bb8344a9) SHA1(8b0e904b937c7f34470ad946076240b0c54bf434) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101332V / 655 / 10 Credit Multiplier/20 Line Multiline
// Rainbow Warriors / NSW/ACT / B - 02/03/00
// Cash Express Hyperlink game
ROM_START( rainwrce )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000bf8
	    0x000000-0x06bb13 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x6c1aaee7
	        Calculated Checksum 0x6c1aaee7  (OK)
	    0x06bb14-0x367863 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x06bb14-0x3fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101332v.u7",  0x000000, 0x80000, CRC(68d9bf78) SHA1(6170ea26ebc732abbc26ba1da35a081c8aa8d154) )
	ROM_LOAD32_WORD( "0101332v.u11", 0x000002, 0x80000, CRC(4170c68d) SHA1(bc00af27bcc176f8d9c9fd0ec1a7139e28f85113) )
	ROM_LOAD32_WORD( "0101332v.u8",  0x100000, 0x80000, CRC(98ebea6f) SHA1(2d78cec777581a87bb4b84e7acd183b237c83e52) )
	ROM_LOAD32_WORD( "0101332v.u12", 0x100002, 0x80000, CRC(b8afd281) SHA1(2d73b5af667d36e8b29e9fc3cc62f220daeffbb9) )
	ROM_LOAD32_WORD( "0101332v.u9",  0x200000, 0x80000, CRC(eb7d7af6) SHA1(a11e8029b0d5ef9bb8c51fea4e9f0a051cdb2eaf) )
	ROM_LOAD32_WORD( "0101332v.u13", 0x200002, 0x80000, CRC(36debb0e) SHA1(4aaa495f74dfb13aa1dc47f3a8af8e54496c1ab8) )
	ROM_LOAD32_WORD( "0101332v.u10", 0x300000, 0x80000, CRC(39f5861f) SHA1(c614ebe2c324d5c3fff32379300f2869fba49d39) )
	ROM_LOAD32_WORD( "0101332v.u14", 0x300002, 0x80000, CRC(92274626) SHA1(fae8d89efba9bf3d171bfe484015d009786ce40d) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100400V / 598/2 / 20 Credit Multiplier / 25 Credit Multiway
// Reel Power / Local / A - 01/11/96
ROM_START( reelpwr )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b80
	    0x000000-0x059d1b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xe1f7a87e
	        Calculated Checksum 0xe1f7a87e  (OK)
	    0x059d1c-0x1bb697 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x059d1c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100400v.u7",  0x000000, 0x80000, CRC(ab7eab54) SHA1(ca2e70b20b2e55e44356a00cbfc5cf5bc681b57e) )
	ROM_LOAD32_WORD( "0100400v.u11", 0x000002, 0x80000, CRC(6b2f608f) SHA1(6e0f713ca0f514d407928b84493e1fead0184513) )
	ROM_LOAD32_WORD( "0100400v.u8",  0x100000, 0x80000, CRC(b727f192) SHA1(eb38d7f4b6b6d210ab0c514adf3b792686ba5fb8) )
	ROM_LOAD32_WORD( "0100400v.u12", 0x100002, 0x80000, CRC(e6033756) SHA1(82603f02fcec2b01f5a1cfc13d50129669e84dc7) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100779V / 628 / 25 Credit Multiway / 8 Credit Multiplier
// Reelin'n Rockin / Local / A - 13/07/98
// ROM says 'Reelin'n Rockin', artwork says 'Reelin-n-Rockin'
ROM_START( reelrock )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ba8
	    0x000000-0x062f6f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x67b49a57
	        Calculated Checksum 0x67b49a57  (OK)
	    0x062f70-0x1a752b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x062f70-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100779v.u7",  0x000000, 0x80000, CRC(b60af34f) SHA1(1143380b765db234b3871c0fe04736472fde7de4) )
	ROM_LOAD32_WORD( "0100779v.u11", 0x000002, 0x80000, CRC(57e341d0) SHA1(9b0d50763bb74ca5fe404c9cd526633721cf6677) )
	ROM_LOAD32_WORD( "0100779v.u8",  0x100000, 0x80000, CRC(57eec667) SHA1(5f3888d75f48b6148f451d7ebb7f99e1a0939f3c) )
	ROM_LOAD32_WORD( "0100779v.u12", 0x100002, 0x80000, CRC(4ac20679) SHA1(0ac732ffe6a33806e4a06e87ec875a3e1314e06b) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101460V / 628/2 / 10, 25 & 50 Credit Multiplier / 20 Line Multiline
// Reelin 'n Rockin / QLD / E - 20/12/00
// ROM says 'Reelin 'n Rockin', artwork says 'Reelin-n-Rockin'
ROM_START( reelrockql )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ad0
	    0x000000-0x08737b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x962948fb
	        Calculated Checksum 0x962948fb  (OK)
	    0x08737c-0x2b2beb is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101460v.u7",  0x000000, 0x80000, CRC(6781085a) SHA1(8fbd8cddada944ca3fd633bdc3b5dc3c0a14736d) )
	ROM_LOAD32_WORD( "0101460v.u11", 0x000002, 0x80000, CRC(43525988) SHA1(705c0114300ea099c7eb96c5035233753534d28d) )
	ROM_LOAD32_WORD( "0101460v.u8",  0x100000, 0x80000, CRC(21767926) SHA1(cbfc3304aab6ef4988652071b263a814804f5f6b) )
	ROM_LOAD32_WORD( "0101460v.u12", 0x100002, 0x80000, NO_DUMP )
	ROM_LOAD32_WORD( "0101460v.u9",  0x200000, 0x80000, CRC(324f6fd8) SHA1(c6d689447eb15a57046bcdecc5dbd67b716112f5) )
	ROM_LOAD32_WORD( "0101460v.u13", 0x200002, 0x80000, NO_DUMP )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0400549V / 608 / 3 Credit Multiplier/3 Line Multiline
// Return of the Samurai / Local / A - 17/04/97
ROM_START( retrsam )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b88
	    0x000000-0x06445b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xb6820a81
	        Calculated Checksum 0xb6820a81  (OK)
	    0x06445c-0x10203b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x06445c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0400549v.u7",  0x000000, 0x80000, CRC(129be82c) SHA1(487639b7d42d6d35a9c48b44d26667c269b5b633) )
	ROM_LOAD32_WORD( "0400549v.u11", 0x000002, 0x80000, CRC(b91f5d4c) SHA1(8116166a759405b97797b4acb2cc3e139bd12de7) )
	ROM_LOAD32_WORD( "0400549v.u8",  0x100000, 0x80000, CRC(8d0e61a8) SHA1(254b106e71a0888b0456afd8d63006d72c0ba292) )
	ROM_LOAD32_WORD( "0400549v.u12", 0x100002, 0x80000, CRC(fdf22d5b) SHA1(664fa003a350c0a3b515b7c384d32176158c2d3e) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200549V / 608 / 3 Credit Multiplier/3 Line Multiline
// Return of the Samurai / Local / A - 17/04/97
// Same strings as retrsam but earlier version
ROM_START( retrsama )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b88
	    0x000000-0x0590b7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xa4b725ab
	        Calculated Checksum 0xa4b725ab  (OK)
	    0x0590b8-0x0ef623 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200549v.u7",  0x000000, 0x80000, CRC(acb913c1) SHA1(eb008b2b3d06f769f1ea1c75b52334e468c5f13c) )
	ROM_LOAD32_WORD( "0200549v.u11", 0x000002, 0x80000, CRC(99f61822) SHA1(88a726a5c9cae3a7d3120cb9013ca4d38ef8c560) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200506V / 608/1 / 10 Credit Multiplier/9 Line Multiline
// Return of the Samurai / Local / A - 28/04/97
ROM_START( retrsamb )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b88
	    0x000000-0x05889b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xd80cf106
	        Calculated Checksum 0xd80cf106  (OK)
	    0x05889c-0x0f313b is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200506v.u7",  0x000000, 0x80000, CRC(e60859a1) SHA1(0be0114a87a21b955dfe24d01547e2d93dcb4f2c) )
	ROM_LOAD32_WORD( "0200506v.u11", 0x000002, 0x80000, CRC(e662404b) SHA1(f0da3384c81d01ec17d24b2191d3a0b0eaf48d12) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200534V / 596/3 / 10 Credit Multiplier/20 Line Multiline
// Rushin Rooster / Local / C - 25/06/97
// ROM says '10 Credit Multiplier' but game has a 5 credit multiplier
ROM_START( rushrst )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x05a0c3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x390046da
	        Calculated Checksum 0x390046da  (OK)
	    0x05a0c4-0x2fa1ab is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200534v.u7",  0x000000, 0x80000, CRC(7e36e609) SHA1(1c08a895c2b3182923d2d637867614aca993f277) )
	ROM_LOAD32_WORD( "0200534v.u11", 0x000002, 0x80000, CRC(f9366606) SHA1(433089dd87fbda68922e5413d2dcfcd3939b626c) )
	ROM_LOAD32_WORD( "0200534v.u8",  0x100000, 0x80000, CRC(6125e6b6) SHA1(018514fc72c379d56a6b1335573b074e03fc7620) )
	ROM_LOAD32_WORD( "0200534v.u12", 0x100002, 0x80000, CRC(bdb1ffe7) SHA1(3ba58ad2e7efc2a6bb060ae82370d6e2ac4fa8ad) )
	ROM_LOAD32_WORD( "0200534v.u9",  0x200000, 0x80000, CRC(d72749be) SHA1(7625bc6776a63b850254295cc8942d4ca08837ef) )
	ROM_LOAD32_WORD( "0200534v.u13", 0x200002, 0x80000, CRC(dfe02424) SHA1(798ce31ef23cf5bd7d5b04dc21ffd99b2f6243b3) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0400501V / 578 / 5 COIN MULTIPLIER
// SUPER BUCKS II / NSW/ACT / G 26/07/99
// Sound data is damaged due to bad u8 ROM
ROM_START( sbuk2 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b98
	    0x000000-0x06ab7f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x874caad2
	        Calculated Checksum 0x874caad2  (OK)
	    0x06ab80-0x1fffef is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x06ab80-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0400501v.u7",  0x000000, 0x80000, CRC(f025775d) SHA1(71a94f6f17fa7cdcd997b0117b8f4afe21606a69) )
	ROM_LOAD32_WORD( "0400501v.u11", 0x000002, 0x80000, CRC(f1b51a61) SHA1(8e9fcb071f704122e13333094828a41974646792) )
	ROM_LOAD32_WORD( "0400501v.u8",  0x100000, 0x80000, BAD_DUMP CRC(03912f4e) SHA1(48bdcd2160e05261b7d834c53e1d483acaad098f) ) // bit 0x20 is stuck on for most of the ROM
	ROM_LOAD32_WORD( "0400501v.u12", 0x100002, 0x80000, CRC(f9b65d2b) SHA1(f519fc284aaa08d3619e4d88e92e690320cf5432) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0300006V / Super Bucks II
// No combination data due to missing u11 ROM
// This version had Superman-style reel symbols, whereas sbuk2 has similar graphics to Super Bucks III
ROM_START( sbuk2a )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to missing ROMs
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0300006v.u7",  0x000000, 0x80000, CRC(d1833c73) SHA1(1576a7877877569438571a16c51fdd56a172c60d) )
	ROM_LOAD32_WORD( "0300006v.u11", 0x000002, 0x80000, NO_DUMP )
	ROM_LOAD32_WORD( "0300006v.u8",  0x100000, 0x80000, CRC(c79a2624) SHA1(ae3cec2fe8bdcd9053ab097b5f1354fb480b4777) )
	ROM_LOAD32_WORD( "0300006v.u12", 0x100002, 0x80000, NO_DUMP )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200711V / 626 / 3 Credit Multiplier / 3 Line Multiline
// Super Bucks III / NSW/ACT / A-22/04/98
ROM_START( sbuk3 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ba8
	    0x000000-0x05ead3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x23d4cb22
	        Calculated Checksum 0x23d4cb22  (OK)
	    0x05ead4-0x114e33 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05ead4-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200711v.u7",  0x000000, 0x80000, CRC(e056c7db) SHA1(7a555583f750d8275b2ffd25a0efbe370a5ac43c) )
	ROM_LOAD32_WORD( "0200711v.u11", 0x000002, 0x80000, CRC(a810782c) SHA1(5d59b464c44ec32b2b977f8326c8bf3424a61e07) )
	ROM_LOAD32_WORD( "0200711v.u8",  0x100000, 0x80000, CRC(2ff83479) SHA1(2f0c6c12e115a5592c29e806a946817a4f1b89a3) )
	ROM_LOAD32_WORD( "0200711v.u12", 0x100002, 0x80000, CRC(a585172d) SHA1(3c74efb11285ff78ce76a7e8af2f936d3dc31290) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100711V / 626 / 3 Credit Multiplier / 3 Line Multiline
// Super Bucks III / NSW/ACT / A-22/04/98
// Same strings as sbuk3 but earlier version
ROM_START( sbuk3a )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ba8
	    0x000000-0x05eaff is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x446525e8
	        Calculated Checksum 0x446525e8  (OK)
	    0x05eb00-0x114e5f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05eb00-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100711v.u7",  0x000000, 0x80000, CRC(2bc355bd) SHA1(754f48ee9929e8d65a2f6cc954e8cdcdcf4a5268) )
	ROM_LOAD32_WORD( "0100711v.u11", 0x000002, 0x80000, CRC(eeb47ed4) SHA1(81c878d2942d0d872311718e8f1b91d65f502cbe) )
	ROM_LOAD32_WORD( "0100711v.u8",  0x100000, 0x80000, CRC(1683ac16) SHA1(5ddba570f6c14ae729acf76705ac7878419fa517) )
	ROM_LOAD32_WORD( "0100711v.u12", 0x100002, 0x80000, CRC(0ce0ba8d) SHA1(7fc6ee6281bb3c474fa0cf4d879e735ae03bb1ed) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1575 / MV4137 / 5,10,20 Credit Multiplier / 20 Line Multiline
// Sweet Liberty Deluxe / Export / A - 11/02/01
// Requires set chips 4.04.xx
ROM_START( sldeluxe )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0ec207 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xa86cc13c
	        Calculated Checksum 0xa86cc13c  (OK)
	    0x0ec208-0x31d6b3 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1575.u7",  0x000000, 0x80000, CRC(5962d463) SHA1(d78c3be7a2aa6d03888e928a5498a65b00ad4135) )
	ROM_LOAD32_WORD( "ahg1575.u11", 0x000002, 0x80000, CRC(93bfa5d9) SHA1(ed7dc7c558bd9889e5b49d81c62a62fd178cdceb) )
	ROM_LOAD32_WORD( "ahg1575.u8",  0x100000, 0x80000, CRC(856797d5) SHA1(f37254ff866fbc3f46c4ba12f7bc7c637ef53123) )
	ROM_LOAD32_WORD( "ahg1575.u12", 0x100002, 0x80000, CRC(ddd6cc1e) SHA1(231e079e44058747f6d80f5d41422a8407ace87f) )
	ROM_LOAD32_WORD( "ahg1575.u9",  0x200000, 0x80000, CRC(b3928eef) SHA1(676d5a1f48b8e71cd8ea0cd8d8ea2de6486b3971) )
	ROM_LOAD32_WORD( "ahg1575.u13", 0x200002, 0x80000, CRC(b4d22493) SHA1(f9d1e974b35f39eedaee34ea4775b86192a11e41) )
	ROM_LOAD32_WORD( "ahg1575.u10", 0x300000, 0x80000, CRC(39f0f9f8) SHA1(51361ab74f1e6ae47acfddbccb220cc5da4725dd) )
	ROM_LOAD32_WORD( "ahg1575.u14", 0x300002, 0x80000, CRC(bd890100) SHA1(c82b2891287429a3e77ccaf9b66139f0548f1902) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100673V / 621/2 / 10 Credit Multiplier
// Silver Wolf / Local / A - 23/03/98
ROM_START( slvrwolf )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ba8
	    0x000000-0x05bd47 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xab7c22ca
	        Calculated Checksum 0xab7c22ca  (OK)
	    0x05bd48-0x1875c3 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05bd48-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100673v.u7",  0x000000, 0x80000, CRC(2f7a41d9) SHA1(931c4c1322c64ab89d6b53b681d39bb181b8e6af) )
	ROM_LOAD32_WORD( "0100673v.u11", 0x000002, 0x80000, CRC(44afdf1f) SHA1(9cc4b0ac21ceeeb47af56ee0e05a7e1feb8e67d7) )
	ROM_LOAD32_WORD( "0100673v.u8",  0x100000, 0x80000, CRC(b4e3d198) SHA1(d01767643bdf7829e0d9f1e9663f12413cc1829c) )
	ROM_LOAD32_WORD( "0100673v.u12", 0x100002, 0x80000, CRC(122816dd) SHA1(5c37b29179262a79d8c68c92035ff14fc2885150) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100405V / 599 / 10 Credit Multiplier / 9 Line Multiline
// Snow Cat / Local / B - 23/12/96
ROM_START( snowcat )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b80
	    0x000000-0x0553db is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xf2744b11
	        Calculated Checksum 0xf2744b11  (OK)
	    0x0553dc-0x0ea4a3 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100405v.u7",  0x000000, 0x80000, CRC(e52c01c2) SHA1(98acf33bbe0e4525a02b581eae7b7caf910f2b96) )
	ROM_LOAD32_WORD( "0100405v.u11", 0x000002, 0x80000, CRC(9f9e2637) SHA1(3d4992cec760360931bc5de400c7a27329f8b953) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200606V / 622 / 10 Credit Multiplier / 9 Line Multiline
// Sumo Spins / Local / A - 08/12/97
ROM_START( sumospin )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b88
	    0x000000-0x05d92b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x94d3401c
	        Calculated Checksum 0x94d3401c  (OK)
	    0x05d92c-0x18f637 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05d92c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200606v.u7",  0x000000, 0x80000, CRC(c3ec9f97) SHA1(62c886cc794de4b915533729c5ea5a71a4b59108) )
	ROM_LOAD32_WORD( "0200606v.u11", 0x000002, 0x80000, CRC(919999fe) SHA1(3d800df5e0abed04c76928b04973ea7c7b02e5d1) )
	ROM_LOAD32_WORD( "0200606v.u8",  0x100000, 0x80000, CRC(eb47f317) SHA1(43ead31e788cce1aa03011f634e939489d965144) )
	ROM_LOAD32_WORD( "0200606v.u12", 0x100002, 0x80000, CRC(ba3eede2) SHA1(708a25af0908a1aa874b3ca4897816c65b0c9178) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200465V / 577/1 / 10 Credit Multiplier/9 Line Multiline
// Sweethearts II / Local / C - 07/09/95
ROM_START( swhr2 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	        Checksum code found at 0x000b68
	        0x000000-0x05b507 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	            Expected Checksum   0x757b4b7c
	            Calculated Checksum 0x757b4b7c  (OK)
	        0x05b508-0x0c43af is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200465v.u7",  0x000000, 0x80000, CRC(23350042) SHA1(fd839a4835358057a5ee1fcaf716f2443461352d) )
	ROM_LOAD32_WORD( "0200465v.u11", 0x000002, 0x80000, CRC(dcf51719) SHA1(1ea07091ce22245f77b6de5dcd994efb94c4ba58) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200004V / 577/1 / 10 Credit Multiplier/9 Line Multiline
// Sweethearts II / Local / C - 07/09/95
// Same strings as swhr2 but earlier version
ROM_START( swhr2a )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ae0
	    0x000000-0x041803 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x4449ca76
	        Calculated Checksum 0x4449ca76  (OK)
	    0x041804-0x0ecbb3 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200004v.u7",  0x000000, 0x80000, CRC(de4d6d77) SHA1(959ffb7d06359870e07cb9d761f0bc0480c45e0c) )
	ROM_LOAD32_WORD( "0200004v.u11", 0x000002, 0x80000, CRC(bde067d7) SHA1(cbf2cbd0644f1daeb5c3cd08d72f3d7aafe521ec) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// PHG0742-02 / MV4061 / 5 Credit Multiplier/5 Line Multiline
// Sweethearts II / Export / A - 29/06/98
// Marked as PHG0742 and 92.252%
// All devices are 27c4002 instead of 27c4096
// No set chips required
ROM_START( swhr2u )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0b31cb is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x0720df2c
	        Calculated Checksum 0x0720df2c  (OK)
	    0x0b31cc-0x155097 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0b31cc-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "phg074202.u7",  0x000000, 0x80000, CRC(2cb692f6) SHA1(d0f9952d01f95dd50078e98652b650b70e1cbb70) )
	ROM_LOAD32_WORD( "phg074202.u11", 0x000002, 0x80000, CRC(287c6a10) SHA1(023e8bd7364ec61fcd113527fa6be47c46faee02) )
	ROM_LOAD32_WORD( "phg074202.u8",  0x100000, 0x80000, CRC(57af1295) SHA1(2fb1a0ab8ea159ea891925a8720d0e88184db6fe) )
	ROM_LOAD32_WORD( "phg074202.u12", 0x100002, 0x80000, CRC(885cde6c) SHA1(a6ccf94ecdae2092a703075c9917d0b4004efa52) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 01J01986 / 577/1 / 10 Credit Multiplier/9 Line Multiline
// Sweethearts II / VENEZUELA / C - 07/09/95
// Game is in Spanish, however audit mode is in English
ROM_START( swhr2v )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x07a763 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x014df7a2
	        Calculated Checksum 0x014df7a2  (OK)
	    0x07a764-0x0e360b is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "01j01986.u7",  0x000000, 0x80000, CRC(f51b2faa) SHA1(dbcfdbee92af5f89a8a2611bbc687ee0cc907642) )
	ROM_LOAD32_WORD( "01j01986.u11", 0x000002, 0x80000, CRC(bd7ead91) SHA1(9f775428a4aa0b0a8ee17aed9be620edc2020c5e) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200319V / 569/12 / 25 Credit Multiplier / 20 Line Multiline
// Thor / Local / B - 14/08/96
ROM_START( thor )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000adc
	    0x000000-0x052b07 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xb6d2ed55
	        Calculated Checksum 0xb6d2ed55  (OK)
	    0x052b08-0x1f7ed7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x052b08-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200319v.u7",  0x000000, 0x80000, CRC(2ebc349e) SHA1(be7485b400eef5bf62aa6c0ff79133575d4d5987) )
	ROM_LOAD32_WORD( "0200319v.u11", 0x000002, 0x80000, CRC(5316d04f) SHA1(6b5e829e54da1debda40f13189a21c5c1b0496e2) )
	ROM_LOAD32_WORD( "0200319v.u8",  0x100000, 0x80000, CRC(7f6eedad) SHA1(391b57639d69ba7822e4749ee25027efd99f08a8) )
	ROM_LOAD32_WORD( "0200319v.u12", 0x100002, 0x80000, CRC(a4b27820) SHA1(75bb554300372c8bccea79ab55aa60688b7597fa) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200333V / 570/9 / 10 Credit Multiplier/9 Line Multiline
// Thunder Heart / Local / A - 14/08/96
ROM_START( thndh )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ae0
	    0x000000-0x054c6f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x5b88c486
	        Calculated Checksum 0x5b88c486  (OK)
	    0x054c70-0x1c0ec7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x054c70-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200333v.u7",  0x000000, 0x80000, CRC(ab24e060) SHA1(c25a86de23a364c8b8249402ce9b867539fda65e) )
	ROM_LOAD32_WORD( "0200333v.u11", 0x000002, 0x80000, CRC(d983aaf2) SHA1(9828f6042834976bf594adbcfe9f15f14c48518c) )
	ROM_LOAD32_WORD( "0200333v.u8",  0x100000, 0x80000, CRC(5c484283) SHA1(292393482ab4903d820cb28a889340cafa075844) )
	ROM_LOAD32_WORD( "0200333v.u12", 0x100002, 0x80000, CRC(522b0459) SHA1(d9f0e94223897ef8935558593b1d261ad953a3ec) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200334V / 597/1 / 3 Credit Multiplier/3 Line Multiline
// Thunder Heart / Local / A - 14/08/96
ROM_START( thndha )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b38
	    0x000000-0x053c2b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xe842b26a
	        Calculated Checksum 0xe842b26a  (OK)
	    0x053c2c-0x1c0ec7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x053c2c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200334v.u7",  0x000000, 0x80000, CRC(01a05a1b) SHA1(1baeb8e9280d5210117c84727eac8841b151430a) )
	ROM_LOAD32_WORD( "0200334v.u11", 0x000002, 0x80000, CRC(cd927b1e) SHA1(17ac7b95fbdf61cebd2013131477d4672d403401) )
	ROM_LOAD32_WORD( "0200334v.u8",  0x100000, 0x80000, CRC(3fad9c98) SHA1(361e13c2711458120353152b3a60dff865c2b74c) )
	ROM_LOAD32_WORD( "0200334v.u12", 0x100002, 0x80000, CRC(77c0d46d) SHA1(a183eca1ebdc305dce75caa311635ec98477909d) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100550V / 594/3 / 3 Credit Multiplier/3 Line Multiline
// Top Banana / Local / A - 18/08/97
ROM_START( topbana )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x05851f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xf76f11d1
	        Calculated Checksum 0xf76f11d1  (OK)
	    0x058520-0x0c0843 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100550v.u7",  0x000000, 0x80000, CRC(9c5e2d66) SHA1(658143706c0e1f3b43b3ec301da1052363fe5244) )
	ROM_LOAD32_WORD( "0100550v.u11", 0x000002, 0x80000, CRC(1c64b3b6) SHA1(80bbc6e3f47ab932e9c07e0c6063197a2d8e81f7) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100782V / 616/1 / 10 Credit Multiplier/20 Line Multiline
// Toucan Tango 200cm / NSW/ACT / A - 17/06/98
ROM_START( toutango )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b88
	    0x000000-0x06766b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x8c1ecffa
	        Calculated Checksum 0x8c1ecffa  (OK)
	    0x06766c-0x3e0083 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x06766c-0x3fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100782v.u7",  0x000000, 0x80000, CRC(4c70120f) SHA1(e43b39c31c14d16ebf962d8dd201a882df74f595) )
	ROM_LOAD32_WORD( "0100782v.u11", 0x000002, 0x80000, CRC(18519789) SHA1(95385207be6e44746b5e78aa5622afb5258419b2) )
	ROM_LOAD32_WORD( "0100782v.u8",  0x100000, 0x80000, CRC(bf358a6f) SHA1(3ae3bcd486f9c6f5f2a799ed3e4f7b177a59465b) )
	ROM_LOAD32_WORD( "0100782v.u12", 0x100002, 0x80000, CRC(fd366efa) SHA1(22a372f5efe43b9320199b7534e9b3a39b582e4a) )
	ROM_LOAD32_WORD( "0100782v.u9",  0x200000, 0x80000, CRC(bc35aed0) SHA1(7ab25c3207c2be43cfefabe4d4bb0a98bc8e5aea) )
	ROM_LOAD32_WORD( "0100782v.u13", 0x200002, 0x80000, CRC(f8a67a69) SHA1(b1a28047cb4572ae15359c30f71cafa4bd70658c) )
	ROM_LOAD32_WORD( "0100782v.u10", 0x300000, 0x80000, CRC(e6528de7) SHA1(b3aa1937f0b673ba2cfa68acc7cb540ebefc66d4) )
	ROM_LOAD32_WORD( "0100782v.u14", 0x300002, 0x80000, CRC(69f2acde) SHA1(cda52548e675a06677a2d9fee89b33f9abb96f64) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0301388V / 616 / 10 Credit Multiplier / 9 Line Multiline
// Toucan Tango 90cm / Holland / C - 11/05/99
ROM_START( toutangonl )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x00104c
	    0x000000-0x060dbf is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xa9e7ac03
	        Calculated Checksum 0xa9e7ac03  (OK)
	    0x060dc0-0x33d693 is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0301388v.u7",  0x000000, 0x80000, CRC(56fa9535) SHA1(660cb91302420dddce3ffeb51f0fcc3bb235acbe) )
	ROM_LOAD32_WORD( "0301388v.u11", 0x000002, 0x80000, CRC(35ee6272) SHA1(8ba9b41f2c614ff0814730cac57456d8a5dabbcb) )
	ROM_LOAD32_WORD( "0301388v.u8",  0x100000, 0x80000, CRC(96110e37) SHA1(43a54721bdfa2be10698ee79fd9a2a4498eae6fa) )
	ROM_LOAD32_WORD( "0301388v.u12", 0x100002, 0x80000, CRC(e00a0b1e) SHA1(353ca6caf8cf3e10b9800a03a97159908903bd44) )
	ROM_LOAD32_WORD( "0301388v.u9",  0x200000, 0x80000, CRC(3ef412b1) SHA1(c5dd849e1b325ec0c9b77092522f5fe538cff4c3) )
	ROM_LOAD32_WORD( "0301388v.u13", 0x200002, 0x80000, CRC(dbba3cfb) SHA1(c836ee3ba8f7cfeb8a099a08407cb6e52880ed32) )
	ROM_LOAD32_WORD( "0301388v.u10", 0x300000, 0x80000, CRC(29c39bb8) SHA1(9c2c16b8a71bac493490bcc0d177c7a762f526b4) )
	ROM_LOAD32_WORD( "0301388v.u14", 0x300002, 0x80000, CRC(fbb37975) SHA1(648b56df5047b8ae60b41bdf29f35f8bf8fe2d29) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 01J00161 / JB001/3 / 25 Credit Multiplier / 20 Line Multiline
// TREASURE TROVE / NSW/ACT / A - 5/10/00
ROM_START( trstrove )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b98
	    0x000000-0x0638d7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x7fa3a1a8
	        Calculated Checksum 0x7fa3a1a8  (OK)
	    0x0638d8-0x158933 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0638d8-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "01j00161.u7",  0x000000, 0x80000, CRC(07a8b338) SHA1(7508d7d0e3494d355cb773165b240ba876a60eec) )
	ROM_LOAD32_WORD( "01j00161.u11", 0x000002, 0x80000, CRC(020a588d) SHA1(4759bef22017fb4c47c87adb6ca7253fdb6bca6b) )
	ROM_LOAD32_WORD( "01j00161.u8",  0x100000, 0x80000, CRC(89a042e7) SHA1(0f95cfd42ce7130176d42c6bbdf8ff22a6662894) )
	ROM_LOAD32_WORD( "01j00161.u12", 0x100002, 0x80000, CRC(715f53cb) SHA1(364c35fc2d36180c13127c8004a8729126f68db1) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0201692V / 692 / 10 or 25 Credit Multiplier/9 or 20 Line Multiline
// Triple Treat / NSW/ACT / A - 17/05/02
ROM_START( tritreat )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x07089b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x56d2b752
	        Calculated Checksum 0x56d2b752  (OK)
	    0x07089c-0x2903cf is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x07089c-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0201692v.u7",  0x000000, 0x80000, CRC(7bc25bba) SHA1(d5f7c3a4bc3c652f57ee4cdbc883ec82069365d1) )
	ROM_LOAD32_WORD( "0201692v.u11", 0x000002, 0x80000, CRC(fbc125b8) SHA1(55dbc3a236804f4a8d26be8e49c29fa5943c5bd6) )
	ROM_LOAD32_WORD( "0201692v.u8",  0x100000, 0x80000, CRC(ef976f78) SHA1(d2c89e8d3bf6af112a99354133f308a5aabad46e) )
	ROM_LOAD32_WORD( "0201692v.u12", 0x100002, 0x80000, CRC(5df3854a) SHA1(2b5175835c587caccafb73a1a5c8abf8f8463cf4) )
	ROM_LOAD32_WORD( "0201692v.u9",  0x200000, 0x80000, CRC(776fbfd2) SHA1(27820dbc6ee1424706aea9c4574da117636fef17) )
	ROM_LOAD32_WORD( "0201692v.u13", 0x200002, 0x80000, CRC(0a0b0ce1) SHA1(41a4d613cf1828df1832c087f0bc18d31076f056) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 01J00851 / JB001/5 / Multi credit / Multi line
// TROJAN HORSE / NSW/ACT / A - 30/10/01
ROM_START( trojhors )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000bb8
	    0x000000-0x06e9f7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x071faa81
	        Calculated Checksum 0x071faa81  (OK)
	    0x06e9f8-0x2df4f7 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x06e9f8-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "01j00851.u7",  0x000000, 0x80000, CRC(7be0caf5) SHA1(b83fba7eb4624b3dc56f763b48b7c45fe31f3396) )
	ROM_LOAD32_WORD( "01j00851.u11", 0x000002, 0x80000, CRC(8c04ed89) SHA1(6727da3a457841e893e27bc8f10d4bb58a61f338) )
	ROM_LOAD32_WORD( "01j00851.u8",  0x100000, 0x80000, CRC(246d3693) SHA1(8c8b893c21e9a486fd36677d7157787bf5d6237b) )
	ROM_LOAD32_WORD( "01j00851.u12", 0x100002, 0x80000, CRC(1eb021a4) SHA1(3195eb5923da018b6c2dac10b70c47aef54dca35) )
	ROM_LOAD32_WORD( "01j00851.u9",  0x200000, 0x80000, CRC(15dee624) SHA1(d678ef7c25419342a1512fab84394e99309009ec) )
	ROM_LOAD32_WORD( "01j00851.u13", 0x200002, 0x80000, CRC(b6d1ceb6) SHA1(b41200620aaa905697ac73b4c86496a53f070ed3) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100269V / 577/3 / 10 Credit Multiplier/9 Line Multiline
// Tropical Delight / Local / B - 15/05/96
ROM_START( trpdlght )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000adc
	    0x000000-0x04ea87 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x698e474c
	        Calculated Checksum 0x698e474c  (OK)
	    0x04ea88-0x1aac5f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x04ea88-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100269v.u7",  0x000000, 0x80000, CRC(b9fc60b4) SHA1(78b6e442209a283c89f7d9da089c9c6adc34a9c2) )
	ROM_LOAD32_WORD( "0100269v.u11", 0x000002, 0x80000, CRC(d9f4e7ec) SHA1(968f51c57451315423284e08a6550d4d77d9a922) )
	ROM_LOAD32_WORD( "0100269v.u8",  0x100000, 0x80000, CRC(a3bf2052) SHA1(2ab6163c6214af49227a1ac560e60332af0c7e84) )
	ROM_LOAD32_WORD( "0100269v.u12", 0x100002, 0x80000, CRC(88978d4e) SHA1(cd2b747fd858f7d84b889bf87865c4fbb349e1b7) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// PHG0625-02 / 577/3 / 10 Credit Multiplier/9 Line Multiline
// Tropical Delight / Export / D - 24/09/97
// Marked as PHG062502 and 92.25%
// All devices are 27c4002 instead of 27c4096
// No set chips required
ROM_START( trpdlghtu )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0b2d1f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x910fae87
	        Calculated Checksum 0x910fae87  (OK)
	    0x0b2d20-0x15384f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0b2d20-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "phg062502.u7",  0x000000, 0x80000, CRC(fe556566) SHA1(fe8c6d699d08577691c766477530abf5cddccf6c) )  // 92.25%
	ROM_LOAD32_WORD( "phg062502.u11", 0x000002, 0x80000, CRC(67e77c32) SHA1(178e6bbe2bb8b55afbe97177920b5f3c4a989adf) )  // 92.25%
	ROM_LOAD32_WORD( "phg062502.u8",  0x100000, 0x80000, CRC(d4858407) SHA1(acf6776f19448648a26aaf53fcb4bc227c546033) )  // base
	ROM_LOAD32_WORD( "phg062502.u12", 0x100002, 0x80000, CRC(0190804b) SHA1(7acb29fb7f77e78bb2afeb74d341475cd089dafb) )  // base

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100791V / 631/1 A / 10 Credit Multiplier / 20 Line Multiline
// UNICORN DREAMING / NSW/ACT / A - 31/08/98
ROM_START( unicornd )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000bf8
	    0x000000-0x05f36f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x2f8bff86
	        Calculated Checksum 0x2f8bff86  (OK)
	    0x05f370-0x1d0a3f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05f370-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100791v.u7",  0x000000, 0x80000, CRC(d785d1b3) SHA1(4aa7c61036dd5fe1cdbc6c39a89881f88f3dd148) )
	ROM_LOAD32_WORD( "0100791v.u11", 0x000002, 0x80000, CRC(b45885f1) SHA1(e32d4afce4e3e62a324173252f559909ea97fe3a) )
	ROM_LOAD32_WORD( "0100791v.u8",  0x100000, 0x80000, CRC(6ba8f7eb) SHA1(bd5b15e22e713095f580b4c371d39af4af9e3289) )
	ROM_LOAD32_WORD( "0100791v.u12", 0x100002, 0x80000, CRC(14afdeda) SHA1(1eb2a297e903dc1a0683425b37669e0af4ae4218) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100813V / 631  A / 10 Credit Multiplier / 9 Line Multiline
// UNICORN DREAMING / NSW/ACT / A - 02/09/98
ROM_START( unicornda )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000bf8
	    0x000000-0x05f087 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x49935fba
	        Calculated Checksum 0x49935fba  (OK)
	    0x05f088-0x1cd29f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05f088-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100813v.u7",  0x000000, 0x80000, CRC(caf69b86) SHA1(7a3bf5dfb687b9452e6f54926656167079fa3ea4) )
	ROM_LOAD32_WORD( "0100813v.u11", 0x000002, 0x80000, CRC(11f7c6f9) SHA1(fa5be6affb543deb9ee37deb4073438f050b240c) )
	ROM_LOAD32_WORD( "0100813v.u8",  0x100000, 0x80000, CRC(a42e0703) SHA1(5ab946d420a92eafd6869e5996b97757d86097e5) )
	ROM_LOAD32_WORD( "0100813v.u12", 0x100002, 0x80000, CRC(b712dcd1) SHA1(f4080185f909d385e93edc73954d05de1dc6aa65) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101228V / MV4113/1 / 10 Credit Multiplier/20 Line Multiline
// Unicorn Dreaming / New Zealand / A- 5/4/00
ROM_START( unicorndnz )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found (uses different startup sequence)
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101228v.u7",  0x000000, 0x80000, CRC(54d55ecb) SHA1(0afb2d6489f01ae55563030e228e1d5443738af1) )
	ROM_LOAD32_WORD( "0101228v.u11", 0x000002, 0x80000, CRC(2be7933c) SHA1(7691d755714bf0801f9ca8510f82a80a4c231178) )
	ROM_LOAD32_WORD( "0101228v.u8",  0x100000, 0x80000, CRC(9c4a6e7f) SHA1(6ac470ec777c68521ef74c66263c8229f8d21176) )
	ROM_LOAD32_WORD( "0101228v.u12", 0x100002, 0x80000, CRC(00d4cd6e) SHA1(eb941164cf421f22bcc9864f198348c4f30d904c) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// BHG1584 / MV4130/1 / 20 Line Multiline / 5, 10, 25, 50 Credit Multiplier
// Unicorn Dreaming / Export / C - 10/17/01
// Requires set chips 4.04.xx
ROM_START( unicorndu )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0ec547 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x16dabc9e
	        Calculated Checksum 0x16dabc9e  (OK)
	    0x0ec548-0x1ee2cb is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0ec548-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "bhg1584.u7",  0x000000, 0x80000, CRC(1490354e) SHA1(9327f01067e4a7f7ce5b6db9373b9ad59533babf) )
	ROM_LOAD32_WORD( "bhg1584.u11", 0x000002, 0x80000, CRC(d2b6745b) SHA1(04b5586e802ce0bd5891b23222bc1db8e7c67f8a) )
	ROM_LOAD32_WORD( "bhg1584.u8",  0x100000, 0x80000, CRC(e604d399) SHA1(9aab86aa6e8590921de9b7234713be72a7c4faa1) )
	ROM_LOAD32_WORD( "bhg1584.u12", 0x100002, 0x80000, CRC(c1e878b5) SHA1(4e4e9dee4d85fbb77dc6f7d39c2dc5b0bf438aef) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 02J02056 / JB009/2 / Multi credit / Multi line
// Venice / VENEZUILA / B - 11/07/02
// This game is downported from the MK6 version
// Venezuela is spelled as 'Venezuila' in the ROM
// Game is in Spanish, however audit mode is in English
ROM_START( venicea5 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code needs to be done
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "02j02056.u7",  0x000000, 0x80000, CRC(2a64d797) SHA1(a23322c38462052f4a892451e68a8c762bf157aa) )
	ROM_LOAD32_WORD( "02j02056.u11", 0x000002, 0x80000, CRC(022bd37b) SHA1(7f64240f3bff1539d787f65f9dc27c6f491d27d3) )
	ROM_LOAD32_WORD( "02j02056.u8",  0x100000, 0x80000, CRC(93ebe05c) SHA1(f7d866687bec14e79d2df63c1ccc6949d3558ba6) )
	ROM_LOAD32_WORD( "02j02056.u12", 0x100002, 0x80000, CRC(c39b5d27) SHA1(a7196709b20cec09d98607e6ec11287d21b51686) )
	ROM_LOAD32_WORD( "02j02056.u9",  0x200000, 0x80000, CRC(35ac80a1) SHA1(b7ceb0fb17f1c4fe2c7e1014c706d4c67566c5c7) )
	ROM_LOAD32_WORD( "02j02056.u13", 0x200002, 0x80000, CRC(5e593f9e) SHA1(9a60aea08c19c613f81dbccecb847fcd4ab93e0f) )
	ROM_LOAD32_WORD( "02j02056.u10", 0x300000, 0x80000, CRC(9bc86c69) SHA1(6aa3ae7e6cdd753efcd8fd6f866c6b83626d0120) )
	ROM_LOAD32_WORD( "02j02056.u14", 0x300002, 0x80000, CRC(7b695bf3) SHA1(89ea3252c9915aab81a85c6c028b21911bf8b4c4) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1535 / MV4076 / 9 Line / 5,10,20 Credit Multiplier
// Wild Africa / Export / C - 17/07/01
// All devices are 27c4096
// Requires set chips 4.04.xx
ROM_START( wafricau )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0e77e7 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x1b3b15a2
	        Calculated Checksum 0x1b3b15a2  (OK)
	    0x0e77e8-0x18d8ab is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0e77e8-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1535.u7",  0x000000, 0x80000, CRC(57c04faa) SHA1(aa86747b21718f9dfa75b41f7280a00f531e13fe) )
	ROM_LOAD32_WORD( "ahg1535.u11", 0x000002, 0x80000, CRC(066917c5) SHA1(a821635edc0199427b43eb324922af99679def21) )
	ROM_LOAD32_WORD( "ahg1535.u8",  0x100000, 0x80000, CRC(64c8c12f) SHA1(3a6e54205c28ae3dfb51ef1895e147e6a9f446a9) )
	ROM_LOAD32_WORD( "ahg1535.u12", 0x100002, 0x80000, CRC(f569843c) SHA1(ccf1b727ad1dc49bca609ee0887b36a29223cace) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200507V / 506/8 / 3 Credit Multiplier/3 Line Multiline
// Wild Amazon / Local / A - 10/10/96
ROM_START( wamazon )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x05c043 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x2c7f1cbb
	        Calculated Checksum 0x2c7f1cbb  (OK)
	    0x05c044-0x0f60cb is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200507v.u7",  0x000000, 0x80000, CRC(44576def) SHA1(3396460444ceb394c9c88e5fc37ccedcfc4b179c) )
	ROM_LOAD32_WORD( "0200507v.u11", 0x000002, 0x80000, CRC(2e24756a) SHA1(247db8316e7815be7524aefc43a5756fad84779a) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200285V / 506/6 / 10 Credit Multiplier
// Wild Amazon / Local / A - 7/5/96
ROM_START( wamazona )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x052b8b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xc07f695c
	        Calculated Checksum 0xc07f695c  (OK)
	    0x052b8c-0x1fffef is the non-Checksummed range still containing data but NOT covered by Checksum  (unusual)
	    0x052b8c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200285v.u7",  0x000000, 0x80000, CRC(bfa21358) SHA1(6b76656401b3dbbace8d4335951468b9885fc7f0) )
	ROM_LOAD32_WORD( "0200285v.u11", 0x000002, 0x80000, CRC(54b2a375) SHA1(635fde5c678b908fa58c0e04ba9b7a84fac1f7fe) )
	ROM_LOAD32_WORD( "0200285v.u8",  0x100000, 0x80000, CRC(4e39f128) SHA1(91d3c6a9e5c30275c3f8967dde55214df097f2ba) )
	ROM_LOAD32_WORD( "0200285v.u12", 0x100002, 0x80000, CRC(fdb10dd3) SHA1(cbf4fe97c75652f83b8ddb929b06941a70b36388) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 01J01996 / 506/8 / 3 Credit Multiplier/3 Line Multiline
// Wild Amazon / VENEZUELA / A - 10/10/96
// Game is in Spanish, however audit mode is in English
ROM_START( wamazonv )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b68
	    0x000000-0x07b2f3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x7b4d5882
	        Calculated Checksum 0x7b4d5882  (OK)
	    0x07b2f4-0x11537b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x07b2f4-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "01j01996.u7",  0x000000, 0x80000, CRC(c14d5f8a) SHA1(66059549f94048fe55ec52a098548e04c2ccece0) )
	ROM_LOAD32_WORD( "01j01996.u11", 0x000002, 0x80000, CRC(6dde9ae6) SHA1(ae18d3fc2269549e60893a3cb828c2993f7f0bfa) )
	ROM_LOAD32_WORD( "01j01996.u8",  0x100000, 0x80000, CRC(00aebc93) SHA1(6829f4d0fc13cb731138c7c54fac90d75f56588b) )
	ROM_LOAD32_WORD( "01j01996.u12", 0x100002, 0x80000, CRC(945e0a05) SHA1(db7580b39c537a7b50898ee99cfceb5eb71be19a) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100553V / 609 / 4 Credit Multiplier / 25 Credit Multiway
// Wicked Winnings / Local / B - 01/07/97
ROM_START( wikwin )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b88
	    0x000000-0x07237f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xadc70321
	        Calculated Checksum 0xadc70321  (OK)
	    0x072380-0x1d1aab is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x072380-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100553v.u7",  0x000000, 0x80000, CRC(f329aa28) SHA1(545cdfe5cc912a2e391c6ba5fb88da4a26336637) )
	ROM_LOAD32_WORD( "0100553v.u11", 0x000002, 0x80000, CRC(55a2583d) SHA1(6455fb8ee21d40d54f32f6bae3e35766f6d4d910) )
	ROM_LOAD32_WORD( "0100553v.u8",  0x100000, 0x80000, CRC(9ad560bd) SHA1(dff9006d27c7bd9b8fe6367133c9897c28a4f3ef) )
	ROM_LOAD32_WORD( "0100553v.u12", 0x100002, 0x80000, CRC(06cf5d68) SHA1(5469e8087371f8b59fd9b2b413682efe2ea0f279) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100297V / 543/8 / 3 Credit Multiplier/3 Line Multiline
// Wild Bill / Local / C - 15/08/96
ROM_START( wildbill )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ad8
	    0x000000-0x054e6b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xd6b20386
	        Calculated Checksum 0xd6b20386  (OK)
	    0x054e6c-0x0ec99f is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100297v.u7",  0x000000, 0x80000, CRC(e3117ab7) SHA1(c13912f524f1c1d373adb6382ceddd1bc18f7f02) )
	ROM_LOAD32_WORD( "0100297v.u11", 0x000002, 0x80000, CRC(57b3c340) SHA1(4f95ed7fed697cf2bfbde8215f6e35768cf20334) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100167V / 569/9 / 20 Credit Multiplier / 9 Line Multiline
// Wild Cougar / Local / B - 27/2/96
ROM_START( wcougar )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000adc
	    0x000000-0x043573 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x0a061a1a
	        Calculated Checksum 0x0a061a1a  (OK)
	    0x043574-0x1061fb is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x043574-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100167v.u7",  0x000000, 0x80000, CRC(47154679) SHA1(21749fbaa60f9bf1db43bdd272e6628ae73bf759) )
	ROM_LOAD32_WORD( "0100167v.u11", 0x000002, 0x80000, CRC(6a5f2c41) SHA1(1365e083d44a373c2d4f17e8e61ec716ffb6d2d5) )
	ROM_LOAD32_WORD( "0100167v.u8",  0x100000, 0x80000, CRC(c262d098) SHA1(87940bd0aef6cb0f5ff21ccda4b209eef8e97eb1) )
	ROM_LOAD32_WORD( "0100167v.u12", 0x100002, 0x80000, CRC(85bb41a7) SHA1(335f29f10f216e202b93b46a376958c3f5271461) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// NHG0296-04 / 569/8 / 10 Credit Multiplier / 9 Line Multiline
// Wild Cougar / Export / D - 19/05/97
// All devices are 27c4002 instead of 27c4096
// No set chips required
ROM_START( wcougaru )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0b0d5b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xdfe9eb92
	        Calculated Checksum 0xdfe9eb92  (OK)
	    0x0b0d5c-0x153803 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0b0d5c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "nhg029604.u7",  0x000000, 0x80000, CRC(7ada053f) SHA1(5102b0b9db505454624750a3fd6db455682538f3) )
	ROM_LOAD32_WORD( "nhg029604.u11", 0x000002, 0x80000, CRC(69a78695) SHA1(1ed89cf38dc85f752449a858cd9558bed235af58) )
	ROM_LOAD32_WORD( "nhg029604.u8",  0x100000, 0x80000, CRC(496b0295) SHA1(237183a192ad9b4bc133014cc83149d4a7062785) )
	ROM_LOAD32_WORD( "nhg029604.u12", 0x100002, 0x80000, CRC(fe2bafdc) SHA1(e8b454db44a532d75b3aff323855340695688f0f) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )

	ROM_REGION16_BE( 0x100, "eeprom0", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom0", 0x000000, 0x000100, CRC(fea8a821) SHA1(c744cac6af7621524fc3a2b0a9a135a32b33c81b) )

	ROM_REGION16_BE( 0x100, "eeprom1", 0 )
	ROM_LOAD16_WORD_SWAP( "eeprom1", 0x000000, 0x000100, CRC(8421e7c2) SHA1(fc1b07d5b7aadafc4a0f2e4dfa698e7c72340717) )

	ROM_REGION( 0x80000, "nvram", 0 )
	ROM_LOAD( "nvram", 0x000000, 0x080000, CRC(dfe52286) SHA1(db31fb64e2fff8aa5ba0cc6d3d73860e8019406c) )

	ROM_REGION( 0x20, "rtc", 0 )
	ROM_LOAD( "rtc", 0x000000, 0x00001f, CRC(6909acb0) SHA1(6a4589599cd1c477e916474e7b029e9a4e92019b) )
ROM_END


// NHG0296-07 / 569/8 / 10 Credit Multiplier / 9 Line Multiline
// Wild Cougar / Export / D - 19/05/97
// Marked 94.982%
// All devices are 27C4002 instead of 27C4096
// No set chips required
ROM_START( wcougarua )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0b0d5b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xe4eced9c
	        Calculated Checksum 0xe4eced9c  (OK)
	    0x0b0d5c-0x153803 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0b0d5c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "nhg029607.u7",  0x000000, 0x80000, CRC(5037aefc) SHA1(afd7e4cf5b53d5ad44dec359cd24a698188c9d35) )  // 94.982%
	ROM_LOAD32_WORD( "nhg029607.u11", 0x000002, 0x80000, CRC(a9c121a7) SHA1(b8133b00f8b42dd7a3be94753ce7482a58aba8f1) )  // 94.982%
	ROM_LOAD32_WORD( "nhg029607.u8",  0x100000, 0x80000, CRC(496b0295) SHA1(237183a192ad9b4bc133014cc83149d4a7062785) )  // base
	ROM_LOAD32_WORD( "nhg029607.u12", 0x100002, 0x80000, CRC(fe2bafdc) SHA1(e8b454db44a532d75b3aff323855340695688f0f) )  // base

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// NHG0296-99 / 569/8 / 10 Credit Multiplier / 9 Line Multiline
// Wild Cougar / Export / D - 19/05/97
// Variation (% and NO): 87,836% 99
// No set chips required
ROM_START( wcougarub )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0b0d5b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xeceefd96
	        Calculated Checksum 0xeceefd96  (OK)
	    0x0b0d5c-0x153803 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0b0d5c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "nhg029699.u7",  0x000000, 0x80000, CRC(cf6465f4) SHA1(ea12ecff7ac4846d09b42480d54bf8cf03ff4c44) )  // 87,836%
	ROM_LOAD32_WORD( "nhg029699.u11", 0x000002, 0x80000, CRC(fd7aa4a1) SHA1(06e02df1b7ee33dff22ba6fcd46657d84fe8a519) )  // 87,836%
	ROM_LOAD32_WORD( "nhg029699.u8",  0x100000, 0x80000, CRC(496b0295) SHA1(237183a192ad9b4bc133014cc83149d4a7062785) )  // base
	ROM_LOAD32_WORD( "nhg029699.u12", 0x100002, 0x80000, CRC(fe2bafdc) SHA1(e8b454db44a532d75b3aff323855340695688f0f) )  // base

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1531 / MV4055 / 5 Line / 10 Credit Multiplier
// Wild Cougar / Export / B - 13/05/97
// Marked as SPC.
// ROM says '5 Line / 10 Credit Multiplier' but this game has 9 lines with a 5, 10 or 20 credit multiplier
// Requires set chips 4.04.xx
ROM_START( wcougaruc )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d18
	    0x000000-0x0e783b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xa27eaf6f
	        Calculated Checksum 0xa27eaf6f  (OK)
	    0x0e783c-0x19287f is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0e783c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1531.u7",  0x000000, 0x80000, CRC(53889e2d) SHA1(f204426aaf07a668878769c12d51f9a721b7bdee) )  // game
	ROM_LOAD32_WORD( "ahg1531.u11", 0x000002, 0x80000, CRC(c138fece) SHA1(4653e4518bdf73da7380da931af85fd41d359a53) )  // game
	ROM_LOAD32_WORD( "ahg1531.u8",  0x100000, 0x80000, CRC(8e2226c5) SHA1(1dc7fff8c56af8102f3de9bbb9d0085546ec877c) )  // base
	ROM_LOAD32_WORD( "ahg1531.u12", 0x100002, 0x80000, CRC(5aac8336) SHA1(d1bfd21579bdcf5b46e0f4db1181d10159f0b643) )  // base

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// AHG1515 / MV4134 / 5,10,25,50 Credit Multiplier / 20 Line Multiline
// Wild Coyote / Export / A - 30/07/01
ROM_START( wcoyote )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to ROMs being corrupted, all files are missing bytes consisting of 0x0D
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "ahg1515.u7",  0x000000, 0x7f070, BAD_DUMP CRC(045858cd) SHA1(232a9631bcdbbd2e60970eca62bdc540e537e1f2) )
	ROM_LOAD32_WORD( "ahg1515.u11", 0x000002, 0x7ff6c, BAD_DUMP CRC(609f181b) SHA1(4cb7c47f0167c0487db419ad7a3c4b4e10a054f3) )
	ROM_LOAD32_WORD( "ahg1515.u8",  0x100000, 0x7fede, BAD_DUMP CRC(25f47c9b) SHA1(cf44f32626107424c8473f275dd6736302763e79) )
	ROM_LOAD32_WORD( "ahg1515.u12", 0x100002, 0x7ff14, BAD_DUMP CRC(0068bce4) SHA1(37517f6bd53660deab471f41a4d63c4b03bf22b3) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200396V / 598/3 / 20 Credit Multiplier / 25 Credit Multiway
// Wizard Ways / Local / A - 04/11/96
// ROM says 1996 but game has newer style music and a 1998+ style denomination sign
ROM_START( wizways )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b88
	    0x000000-0x05ee9b is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x826ee6ad
	        Calculated Checksum 0x826ee6ad  (OK)
	    0x05ee9c-0x17afb3 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x05ee9c-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200396v.u7",  0x000000, 0x80000, CRC(85fca945) SHA1(ca48a907b4f6c1b665ae053e2992681f02166cb1) )
	ROM_LOAD32_WORD( "0200396v.u11", 0x000002, 0x80000, CRC(677c855f) SHA1(a4ec8e6151271af292379ead28214ef9163bfdc3) )
	ROM_LOAD32_WORD( "0200396v.u8",  0x100000, 0x80000, CRC(4b1192ae) SHA1(2537249ccfc8c507762ac2c46d05ef13fa3d0bf9) )
	ROM_LOAD32_WORD( "0200396v.u12", 0x100002, 0x80000, CRC(934d7286) SHA1(205f72b62d83667e9068141346bda3bcb9742a83) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0100337V / 600 / 10 Credit Multiplier/9 Line Multiline
// Wild Angels / Local / B - 24/09/96
ROM_START( wldangel )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000ae0
	    0x000000-0x05259f is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0xac12cd9a
	        Calculated Checksum 0xac12cd9a  (OK)
	    0x0525a0-0x1cc0bf is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0525a0-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0100337v.u7",  0x000000, 0x80000, CRC(46b76cce) SHA1(6188a96c20aa9f7ded8dc7088ac5dc6dfc0afaa7) )
	ROM_LOAD32_WORD( "0100337v.u11", 0x000002, 0x80000, CRC(396fcc02) SHA1(cd19db425a664a49379cbb640215f258a8137902) )
	ROM_LOAD32_WORD( "0100337v.u8",  0x100000, 0x80000, CRC(239d19e4) SHA1(c17f8e3c16d0280a291490f69a51f3f6e2177ac7) )
	ROM_LOAD32_WORD( "0100337v.u12", 0x100002, 0x80000, CRC(81f0d1c8) SHA1(1e91c9457593f592c0ca4a186a49b00b2cfe256f) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// RHG0418-04 / 541/2 / 10 Credit Multiplier / 5 Line Multiline
// Winning Post / Export / G - 11/02/97
// No set chips required
ROM_START( wnpost )
	ARISTOCRAT_MK5_USA_SETCHIPS
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d08
	    0x000000-0x0c3697 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x162990b8
	        Calculated Checksum 0x162990b8  (OK)
	    0x0c3698-0x1e00fb is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0c3698-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "rhg041804.u7",  0x000000, 0x80000, CRC(73274802) SHA1(9838a63d5f4e1bf31675ac15c34a17e709f2f647) )
	ROM_LOAD32_WORD( "rhg041804.u11", 0x000002, 0x80000, CRC(4f076a94) SHA1(2962429c930ffc17e2f37a006215ee6f1c649953) )
	ROM_LOAD32_WORD( "rhg041804.u8",  0x100000, 0x80000, CRC(b1830ffa) SHA1(c23a97bfdfe9e408f0ec8053646d6c8c8e06a263) )
	ROM_LOAD32_WORD( "rhg041804.u12", 0x100002, 0x80000, CRC(ff70b305) SHA1(3c832ed20b1d00318fd3d2428bb469735f68965b) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0101158V / 608/4 / 3 Credit Multiplier/3 Line Multiline
// Wild Thing / NSW/ACT / B - 14/12/99
ROM_START( wthing )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000b74
	    0x000000-0x0673cb is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x89dd307a
	        Calculated Checksum 0x89dd307a  (OK)
	    0x0673cc-0x1b367b is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x0673cc-0x1fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0101158v.u7",  0x000000, 0x80000, CRC(eb402ffb) SHA1(49ef6ca2503a6e785f62cb29e505e5c2ba019e37) )
	ROM_LOAD32_WORD( "0101158v.u11", 0x000002, 0x80000, CRC(61d22f2e) SHA1(b836e5afbd5bb14ae68e100a6042f1953ed57a21) )
	ROM_LOAD32_WORD( "0101158v.u8",  0x100000, 0x80000, CRC(f21153b8) SHA1(24830b3939a8568b0d5b59d4fdbd2d9e7b46a6d7) )
	ROM_LOAD32_WORD( "0101158v.u12", 0x100002, 0x80000, CRC(450a4f4f) SHA1(e59fb3260755a125c47ff6c1a042a48b0eace72d) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0201176V / 608/5 / 25 Credit Multiplier/20 Line Multiline
// Wild Thing / NSW/ACT / B - 25/02/00
ROM_START( wthinga )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	// checksum code not found due to ROMs being corrupted, all files are missing bytes consisting of 0x0D
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0201176v.u7",  0x000000, 0x7f44a, BAD_DUMP CRC(e2632da7) SHA1(ff53d87d8f45c3bcece358d0ecfa89e6912e1ccf) )
	ROM_LOAD32_WORD( "0201176v.u11", 0x000002, 0x7faa7, BAD_DUMP CRC(aa8252ee) SHA1(f60d36269d26b9eee4b9f0bcd8eab028c58b81c8) )
	ROM_LOAD32_WORD( "0201176v.u8",  0x100000, 0x7f5eb, BAD_DUMP CRC(0c5768aa) SHA1(0048e73bb6274b8586f31fe2e23209bb1910ae96) )
	ROM_LOAD32_WORD( "0201176v.u12", 0x100002, 0x7f63c, BAD_DUMP CRC(9fe934e4) SHA1(0178226594fcc9c140f00c8272cca9e3be19dda2) )
	ROM_LOAD32_WORD( "0201176v.u9",  0x200000, 0x7ffd6, BAD_DUMP CRC(633eff32) SHA1(12314db898f8e553cf5ae099cd8b2b9f0c4da3c6) )
	ROM_LOAD32_WORD( "0201176v.u13", 0x200002, 0x7ffdb, BAD_DUMP CRC(026317bc) SHA1(94a48b33ddc60d6271ac0a89fc86b9f1be68f9a6) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 0200954V / 638/1 / 10 Credit Multiplier / 20 Line Multiline
// White Tiger Classic / NSW/ACT / B - 08/07/99
ROM_START( wtiger )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000d30
	    0x000000-0x060227 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x61da8e77
	        Calculated Checksum 0x61da8e77  (OK)
	    0x060228-0x0d61cf is the non-Checksummed range (unusual endpoint)
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "0200954v.u7",  0x000000, 0x80000, CRC(752e54c5) SHA1(9317544a7cf2d9bf29347d31fe72331fc3d018ef) )
	ROM_LOAD32_WORD( "0200954v.u11", 0x000002, 0x80000, CRC(38e888b1) SHA1(acc857eb2be19140bbb58d70583e08f24807b9f2) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


// 03J00191 / JB005/1 / Multi credit / Multi line
// Yukon Gold / NSW/ACT / A - 30/10/2000
ROM_START( yukongl5 )
	ARISTOCRAT_MK5_GALS
	ARISTOCRAT_MK5_EEPROM
	/*
	    Checksum code found at 0x000bb8
	    0x000000-0x06dbc3 is the Checksummed Range (excluding 0x000020-0x000027 where Checksum is stored)
	        Expected Checksum   0x9a99028b
	        Calculated Checksum 0x9a99028b  (OK)
	    0x06dbc4-0x2cb767 is the non-Checksummed range still containing data but NOT covered by Checksum
	    0x06dbc4-0x2fffff is the non-Checksummed range if the additional vectors? at the end are included
	*/
	ROM_REGION( 0x400000, "game_prg", ROMREGION_ERASEFF )
	ROM_LOAD32_WORD( "03j00191.u7",  0x000000, 0x80000, CRC(b3c34f04) SHA1(ee8e1c9d04f35420f9e4e97520e9aef07c6b73da) )
	ROM_LOAD32_WORD( "03j00191.u11", 0x000002, 0x80000, CRC(2b9d4a60) SHA1(22afc2e5fed784ba335d83fcc31b2490fc5d0663) )
	ROM_LOAD32_WORD( "03j00191.u8",  0x100000, 0x80000, CRC(0e732007) SHA1(d5c7dcee6a3e99522c7612a88b5b242f31f296be) )
	ROM_LOAD32_WORD( "03j00191.u12", 0x100002, 0x80000, CRC(6b857ea5) SHA1(908584918f756107e8545fbb52abce7ea1b82b34) )
	ROM_LOAD32_WORD( "03j00191.u9",  0x200000, 0x80000, CRC(8ccbbec3) SHA1(cd0a3d2c3437f1a00cee5b992e7365e7df10b8b7) )
	ROM_LOAD32_WORD( "03j00191.u13", 0x200002, 0x80000, CRC(e514b87f) SHA1(5423215bc03ab8468d5ebec0dba6ba7820cdcd50) )

	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 ) /* ARM Code */
	ROM_REGION( 0x200000, "vram", ROMREGION_ERASE00 )
	ROM_REGION( 0x20000*4, "sram", ROMREGION_ERASE00 )
ROM_END


/*************************
*      Game Drivers      *
*************************/

//     YEAR  NAME         PARENT    MACHINE             INPUT         CLASS           INIT           ROT   COMPANY       FULLNAME                                                FLAGS
GAMEL( 1995, aristmk5,    0,        aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "MKV Set/Clear Chips (USA)",                            MACHINE_NOT_WORKING | MACHINE_IS_BIOS_ROOT, layout_aristmk5_us )

// Dates listed below are for the combination (reel layout), not release dates
GAMEL( 1998, adonis,      aristmk5, aristmk5,           aristmk5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Adonis (0200751V, NSW/ACT)",                           MACHINE_NOT_WORKING, layout_aristmk5 )        // 602/9, A - 25/05/98, Rev 10
GAMEL( 1998, adonisa,     adonis,   aristmk5,           aristmk5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Adonis (0100751V, NSW/ACT)",                           MACHINE_NOT_WORKING, layout_aristmk5 )        // 602/9, A - 25/05/98, Rev 9
GAMEL( 1999, adonisce,    adonis,   aristmk5,           adonisce,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Adonis - Cash Express (0201005V, NSW/ACT)",            MACHINE_NOT_WORKING, layout_adonisce )        // 602/9, C - 06/07/99, Rev 12
GAMEL( 2001, adonisu,     adonis,   aristmk5_usa,       bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Adonis (BHG1508, US)",                                 MACHINE_NOT_WORKING, layout_adonisu )         // MV4124/1, B - 31/07/01
GAMEL( 2002, alchemst,    aristmk5, aristmk5,           goldenra,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Alchemist (01J02046, Venezuela)",                      MACHINE_NOT_WORKING, layout_yukongl5 )        // JB013/1, A - 22/01/02, Rev 17
GAMEL( 2000, bparty,      aristmk5, aristmk5_usa_touch, bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Bachelorette Party (BHG1248, US)",                     MACHINE_NOT_WORKING, layout_bparty )          // MV4119/1, B - 25/08/2000
GAMEL( 1996, baddog,      aristmk5, aristmk5,           baddog,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Bad Dog Poker (0200428V, NSW/ACT)",                    0,                   layout_baddog )          // 386/56, A - 17/12/96
GAMEL( 1998, bootsctn,    aristmk5, aristmk5,           cashcham,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Boot Scootin' (0100812V, NSW/ACT)",                    MACHINE_NOT_WORKING, layout_cashcham )        // 616/1, B - 11/12/98, Rev 20
GAMEL( 1999, bootsctnu,   bootsctn, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Boot Scootin' (GHG1012-02, US)",                       MACHINE_NOT_WORKING, layout_aristmk5_us )     // MV4098, A - 25/08/99
GAMEL( 1999, bootsctnua,  bootsctn, aristmk5_usa,       pengpuck,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Boot Scootin' (GHG1008-03, US)",                       MACHINE_NOT_WORKING, layout_aristmk5_us_200 ) // MV4098/1, A - 27/07/99
GAMEL( 2001, bootsctnub,  bootsctn, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Boot Scootin' (AHG1547, US)",                          MACHINE_NOT_WORKING, layout_adonisu )         // MV4098/1, B - 22/03/01
GAMEL( 1996, bumblbug,    aristmk5, aristmk5,           swhr2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Bumble Bugs (0200510V, NSW/ACT)",                      MACHINE_NOT_WORKING, layout_swhr2 )           // 593, D - 5/07/96, Rev 3
GAMEL( 1997, bumblbugu,   bumblbug, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Bumble Bugs (CHG0479-03, US)",                         MACHINE_NOT_WORKING, layout_aristmk5_us )     // 593, D - 05/07/97
GAMEL( 1997, bumblbugua,  bumblbug, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Bumble Bugs (CHG0479-99, US)",                         MACHINE_NOT_WORKING, layout_aristmk5_us )     // 593, D - 05/07/97
GAMEL( 1995, buttdeli,    aristmk5, aristmk5,           swhr2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Butterfly Delight (0200143V, NSW/ACT)",                0,                   layout_swhr2 )           // 571/4, A - 19/12/95, Rev 1.8.1.0
GAMEL( 1998, cashcat,     aristmk5, aristmk5,           aristmk5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Cash Cat (0100676V, NSW/ACT)",                         MACHINE_NOT_WORKING, layout_aristmk5 )        // 614/3, A - 03/04/98, Rev 7
GAMEL( 1997, cashcata,    cashcat,  aristmk5,           aristmk5_9,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Cash Cat (0100557V, NSW/ACT)",                         MACHINE_NOT_WORKING, layout_dolphntrb )       // 614/1, B - 01/12/97, Rev 7
GAMEL( 1999, cashcatnz,   cashcat,  aristmk5,           cashcatnz,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Cash Cat (0300863V, New Zealand)",                     0,                   layout_cashcatnz )       // MV4089, A - 4/1/99, Rev 26
GAMEL( 1997, cashcham,    aristmk5, aristmk5,           cashcham,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Cash Chameleon (0100438V, NSW/ACT)",                   0,                   layout_cashcham )        // 603/1, C  - 15/4/97, Rev 1.25.3.0
GAMEL( 1998, cashchama,   cashcham, aristmk5,           cashchama,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Cash Chameleon (0200437V, NSW/ACT)",                   MACHINE_NOT_WORKING, layout_cashchama )       // 603(a), D - 18/02/98, Rev 3
GAMEL( 1998, cashchamnz,  cashcham, aristmk5,           cashchamnz,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Cash Chameleon (0300781V, New Zealand)",               0,                   layout_cashchamnz )      // MV4067, A - 31/08/98, Rev 21
GAMEL( 1996, cashchamu,   cashcham, aristmk5_usa,       bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Cash Chameleon (DHG4078-99, US)",                      MACHINE_NOT_WORKING, layout_cashchamu )       // 603(a), B - 06/12/96
GAMEL( 1997, cashcra5,    aristmk5, aristmk5,           aristmk5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Cash Crop (0300467V, NSW/ACT)",                        MACHINE_NOT_WORKING, layout_aristmk5 )        // 607, C - 14/07/97, Rev 6
GAMEL( 1998, chariotc,    aristmk5, aristmk5,           chariotc,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "The Chariot Challenge (0100787V, NSW/ACT)",            MACHINE_NOT_WORKING, layout_aristmk5 )        // 630/1, A - 10/08/98, Rev 11
GAMEL( 1998, chariotcv,   chariotc, aristmk5,           chariotcv,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "The Chariot Challenge (04J00714, Venezuela)",          MACHINE_NOT_WORKING, layout_snowcat )         // 630, A - 10/08/98, Rev 12
GAMEL( 2001, checkma5,    aristmk5, aristmk5,           checkma5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Checkmate (01J00681, NSW/ACT)",                        MACHINE_NOT_WORKING, layout_checkma5 )        // JB011, B - 06/07/01, Rev 17
GAMEL( 1996, chickna5,    aristmk5, aristmk5,           chickna5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Chicken (0100351V, NSW/ACT)",                          0,                   layout_snowcat )         // 596, A - 27/08/96, Rev 1.24
GAMEL( 1998, chickna5u,   chickna5, aristmk5_usa,       chickna5u,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Chicken (RHG0730-03, US)",                             MACHINE_NOT_WORKING, layout_aristmk5_us )     // 596, C - 23/02/98
GAMEL( 1998, chickna5ua,  chickna5, aristmk5_usa,       chickna5u,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Chicken (RHG0730-99, US)",                             MACHINE_NOT_WORKING, layout_aristmk5_us )     // 596, C - 23/02/98
GAMEL( 1998, coralrc2,    aristmk5, aristmk5,           coralrc2,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Coral Riches II (0100919V, NSW/ACT)",                  MACHINE_NOT_WORKING, layout_coralrc2 )        // 577/7, A - 29/12/98, Rev 12
GAMEL( 1998, cuckoo,      aristmk5, aristmk5,           aristmk5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Cuckoo (0200753V, NSW/ACT)",                           MACHINE_NOT_WORKING, layout_aristmk5 )        // 615/1, D - 03/07/98, Rev 6
GAMEL( 2000, cuckoou,     cuckoo,   aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Cuckoo (CHG1195, US)",                                 MACHINE_NOT_WORKING, layout_cuckoou )         // MV4104, C - 02/02/00
GAMEL( 1995, dstbloom,    aristmk5, aristmk5,           wcougar,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Desert Bloom (0300111V, NSW/ACT)",                     0,                   layout_wcougar )         // 577/2, A - 12/10/95, Rev 1.16.4.0
GAMEL( 1995, dstblooma,   dstbloom, aristmk5,           wcougar,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Desert Bloom (0200111V, NSW/ACT)",                     0,                   layout_wcougar )         // 577/2, A - 12/10/95, Rev 1.16.4.0
GAMEL( 1999, diamdove,    aristmk5, aristmk5,           retrsam,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Diamond Dove (0101018V, NSW/ACT)",                     MACHINE_NOT_WORKING, layout_sbuk3 )           // 640, B - 19/05/99, Rev 8
GAMEL( 1996, dmdfever,    aristmk5, aristmk5,           wildbill,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Diamond Fever (0200302V, NSW/ACT)",                    0,                   layout_wildbill )        // 483/7, E - 05/09/96, Rev 1.23.5.0
GAMEL( 1997, dimtouch,    aristmk5, aristmk5_touch,     dimtouch,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Diamond Touch (0400433V, NSW/ACT)",                    MACHINE_NOT_WORKING, layout_dimtouch )        // 604, E - 30/06/97, Rev 17
GAMEL( 1996, dolphntr,    aristmk5, aristmk5,           aristmk5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Dolphin Treasure (0200424V, NSW/ACT)",                 MACHINE_NOT_WORKING, layout_aristmk5 )        // 602/1, B - 06/12/96, Rev 3
GAMEL( 1996, dolphntra,   dolphntr, aristmk5,           aristmk5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Dolphin Treasure (0100424V, NSW/ACT)",                 0,                   layout_aristmk5 )        // 602/1, B - 06/12/96, Rev 1.24.4.0
GAMEL( 1996, dolphntrb,   dolphntr, aristmk5,           aristmk5_9,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Dolphin Treasure (0100388V, NSW/ACT)",                 0,                   layout_dolphntrb )       // 602, B - 10/12/96, Rev 1.24.4.0
GAMEL( 1996, dolphntru,   dolphntr, aristmk5_usa,       dolphntru,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Dolphin Treasure (FHG4077-02, US)",                    MACHINE_NOT_WORKING, layout_aristmk5_us )     // 602/1, B - 06/12/96
GAMEL( 1999, dolphntrce,  dolphntr, aristmk5_usa,       dolphntrce,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Dolphin Treasure - Cash Express (AHG1607, US)",        MACHINE_NOT_WORKING, layout_adonisu )         // MV4090, D - 22/12/99
GAMEL( 1999, dolphntrcea, dolphntr, aristmk5_usa,       dolphntru,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Dolphin Treasure - Cash Express (AHG1606, US)",        MACHINE_NOT_WORKING, layout_marmagicu )       // MV4090, D - 22/12/99
GAMEL( 1999, dolphntrceb, dolphntr, aristmk5_usa,       dolphntrce,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Dolphin Treasure - Cash Express (AHG1519, US)",        MACHINE_NOT_WORKING, layout_dolphntrceb )     // MV4090, D - 22/12/99
GAMEL( 1997, drgneye,     aristmk5, aristmk5,           snowcat,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Dragon's Eye (0100521V, NSW/ACT)",                     0,                   layout_snowcat )         // 610, A - 09/05/97, Rev 1.27
GAMEL( 1997, dreamwv,     aristmk5, aristmk5_touch,     dreamwv,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Dream Weaver (0200586V, NSW/ACT)",                     MACHINE_NOT_WORKING, layout_dreamwv )         // 606/2, A - 20/06/97, Rev 16
GAMEL( 2000, dynajack,    aristmk5, aristmk5,           dynajack,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Dynamite Jack (01J00081, NSW/ACT)",                    MACHINE_NOT_WORKING, layout_dynajack )        // JB004, A - 12/07/2000, Rev 17
GAMEL( 1998, eldorda5,    aristmk5, aristmk5,           reelrock,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "El Dorado (0100652V, NSW/ACT)",                        MACHINE_NOT_WORKING, layout_reelrock )        // 623, B - 24/03/98, Rev 3
GAMEL( 1995, eforsta5,    aristmk5, aristmk5,           swhr2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Enchanted Forest (0400122V, NSW/ACT)",                 0,                   layout_swhr2 )           // 570/3, E - 23/06/95, Rev 1.18
GAMEL( 1997, eforsta5u,   eforsta5, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Enchanted Forest (JHG0415-03, US)",                    MACHINE_NOT_WORKING, layout_aristmk5_us )     // MV4033, B - 10/02/97
GAMEL( 1997, eforsta5ua,  eforsta5, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Enchanted Forest (JHG0415-99, US)",                    MACHINE_NOT_WORKING, layout_aristmk5_us )     // MV4033, B - 10/02/97
GAMEL( 2000, eforsta5cea, eforsta5, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Enchanted Forest - Cash Express (AHG1615, US)",        MACHINE_NOT_WORKING, layout_magimaska )       // MV4108/6, C - 17/01/00
GAMEL( 1998, fastfort,    aristmk5, aristmk5,           wildbill,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Fast Fortune (0100651V, NSW/ACT)",                     MACHINE_NOT_WORKING, layout_wildbill )        // 624, D - 07/05/98, Rev 5
GAMEL( 2000, fortellr,    aristmk5, aristmk5,           goldenra,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Fortune Teller (01J00131, NSW/ACT)",                   MACHINE_NOT_WORKING, layout_fortellr )        // JB006, D - 24/11/2000, Rev 17
GAMEL( 1998, gambler,     aristmk5, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "The Gambler (EHG0916-02, US)",                         MACHINE_NOT_WORKING, layout_aristmk5_us )     // MV4084/1, A - 30/10/98
GAMEL( 2001, geisha,      aristmk5, aristmk5,           geisha,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Geisha (0101408V, New Zealand)",                       0,                   layout_geisha )          // MV4127, A - 05/03/01, Rev 25
GAMEL( 1999, genmagi,     aristmk5, aristmk5_touch,     genmagi,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Genie Magic (0200894V, NSW/ACT)",                      MACHINE_NOT_WORKING, layout_genmagi )         // 632/1, C - 15/02/99, Rev 20
GAMEL( 2001, glizrdce,    aristmk5, aristmk5_usa,       bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Green Lizard - Cash Express (AHG1623, US)",            MACHINE_NOT_WORKING, layout_adonisu )         // MV4112/2 - A - 05/01/01
GAMEL( 1998, gnomeatw,    aristmk5, aristmk5,           kgalah,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Gnome Around The World (0100767V, NSW/ACT)",           MACHINE_NOT_WORKING, layout_kgalah )          // 625, C - 18/12/98, Rev 16
GAMEL( 1997, goldpyr,     aristmk5, aristmk5_usa,       dolphntru,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Golden Pyramids (AHG1205-03, US)",                     MACHINE_NOT_WORKING, layout_aristmk5_us )     // MV4091, B - 13/05/97
GAMEL( 1997, goldpyra,    goldpyr,  aristmk5_usa,       bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Golden Pyramids (AHG1206-99, US)",                     MACHINE_NOT_WORKING, layout_aristmk5_us_200 ) // 602/2, B - 13/05/97
GAMEL( 2000, goldenra,    aristmk5, aristmk5,           goldenra,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Golden Ra (0101164V, NSW/ACT)",                        MACHINE_NOT_WORKING, layout_goldenra )        // 661, A - 10/04/00, Rev 15
GAMEL( 2001, hnktonku,    aristmk5, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Honky Tonk (BHG1455, US)",                             MACHINE_NOT_WORKING, layout_magimaska )       // MV4122/3, A - 07/08/01
GAMEL( 2000, honeypot,    aristmk5, aristmk5,           goldenra,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Honey Pot (03J00241, NSW/ACT)",                        MACHINE_NOT_WORKING, layout_yukongl5 )        // JB008, A - 21/11/2000, Rev 17
GAMEL( 1999, incasun,     aristmk5, aristmk5,           incasun,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Inca Sun (0100872V, NSW/ACT)",                         MACHINE_NOT_WORKING, layout_incasun )         // 631/3 B, B - 03/05/99, Rev 15
GAMEL( 1999, incasunsp,   incasun,  aristmk5,           incasun,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Inca Sun (0100872V, NSW/ACT, Show Program)",           MACHINE_NOT_WORKING, layout_incasun )         // 631/3 B, B - 03/05/99, Rev 15
GAMEL( 2000, incasunnz,   incasun,  aristmk5,           incasunnz,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Inca Sun (0101108V, New Zealand)",                     0,                   layout_incasunnz )       // MV4113, A - 6/3/00, Rev 25
GAMEL( 2000, incasunu,    incasun,  aristmk5_usa,       dolphntrce,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Inca Sun (CHG1458, US)",                               MACHINE_NOT_WORKING, layout_adonisu )         // MV4130/3, A - 05/09/00
GAMEL( 2000, incasunua,   incasun,  aristmk5_usa,       bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Inca Sun (DHG1577, US)",                               MACHINE_NOT_WORKING, layout_adonisu )         // MV4130, A - 05/09/00
GAMEL( 1999, incasunv,    incasun,  aristmk5,           incasun,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Inca Sun (01J01946, Venezuela)",                       MACHINE_NOT_WORKING, layout_incasun )         // 631/3 B, B- 03/05/99, Rev 15
GAMEL( 1998, indrema5,    aristmk5, aristmk5,           indrema5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Indian Dreaming (0100845V, NSW/ACT)",                  MACHINE_NOT_WORKING, layout_indrema5 )        // 628/1, B - 15/12/98, Rev 7
GAMEL( 1996, jumpjoey,    aristmk5, aristmk5,           cashcham,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Jumpin' Joey (0100383V, NSW/ACT)",                     0,                   layout_cashcham )        // 586/6, C - 13/11/96
GAMEL( 1995, kgalah,      aristmk5, aristmk5,           kgalah,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "King Galah (0200536V, NSW/ACT)",                       MACHINE_NOT_WORKING, layout_kgalah )          // 613/6, A - 21/07/95
GAMEL( 1995, kgalaha,     kgalah,   aristmk5,           kgalah,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "King Galah (0100536V, NSW/ACT)",                       0,                   layout_kgalah )          // 613, A - 21/07/95
GAMEL( 2001, kgalahce,    kgalah,   aristmk5_usa,       bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "King Galah - Cash Express (AHG1625, US)",              MACHINE_NOT_WORKING, layout_adonisu )         // MV4112/2, B - 07/02/01
GAMEL( 1994, kgbirda5,    aristmk5, aristmk5,           kgbirda5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "K.G. Bird (0200024V, NSW/ACT)",                        0,                   layout_kgbirda5 )        // 540/3, D - 10/10/94
GAMEL( 2001, koalamnt,    aristmk5, aristmk5_usa,       dolphntrce,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Koala Mint (CHG1573, US)",                             MACHINE_NOT_WORKING, layout_adonisu )         // MV4137, A - 12/09/01
GAMEL( 1998, kookabuk,    aristmk5, aristmk5,           aristmk5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Kooka Bucks (0100677V, NSW/ACT)",                      MACHINE_NOT_WORKING, layout_aristmk5 )        // 661, A - 03/04/98, Rev 5
GAMEL( 2000, kyhatonu,    aristmk5, aristmk5_usa_touch, bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Keep Your Hat On (BHG1204, US)",                       MACHINE_NOT_WORKING, layout_bparty )          // MV4114/1, B - 08/05/2000
GAMEL( 1997, locoloot,    aristmk5, aristmk5,           cashcham,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Loco Loot (0100473V, NSW/ACT)",                        0,                   layout_cashcham )        // 599/3, C - 17/06/97, Rev 1.26.13.0
GAMEL( 1997, locoloota,   locoloot, aristmk5,           locoloota,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Loco Loot (0100472V, NSW/ACT)",                        0,                   layout_locoloota )       // 599/2, C - 17/06/97, Rev 1.26.13.0
GAMEL( 1998, locolootnz,  locoloot, aristmk5,           cashchamnz,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Loco Loot (0600725V, New Zealand)",                    0,                   layout_cashchamnz )      // MV4064, A - 8/7/98, Rev 24
GAMEL( 2001, locolootu,   locoloot, aristmk5_usa,       bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Loco Loot (AHG1513, US)",                              MACHINE_NOT_WORKING, layout_adonisu )         // MV4134, A - 30/07/01
GAMEL( 1997, lonewolf,    aristmk5, aristmk5,           wildbill,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Lone Wolf (0100587V, NSW/ACT)",                        0,                   layout_wildbill )        // 621, A - 29/10/97, Rev 1.27.17.0
GAMEL( 1995, luckyclo,    aristmk5, aristmk5,           wcougar,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Lucky Clover (0300109V, NSW/ACT)",                     0,                   layout_wcougar )         // 570/6, A - 12/10/95
GAMEL( 2000, magimask,    aristmk5, aristmk5_usa_touch, bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Magic Mask (AHG1549, US)",                             MACHINE_NOT_WORKING, layout_adonisu )         // MV4115_1, A - 09/05/00
GAMEL( 2000, magimaska,   magimask, aristmk5_usa_touch, aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Magic Mask (AHG1548, US)",                             MACHINE_NOT_WORKING, layout_magimaska )       // MV4115, A - 09/05/00
GAMEL( 2000, magimaskb,   magimask, aristmk5_usa_touch, aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Magic Mask (DHG1309, US)",                             MACHINE_NOT_WORKING, layout_magimaska )       // MV4115, A - 09/05/00
GAMEL( 1997, magtcha5,    aristmk5, aristmk5_touch,     dimtouch,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Magic Touch (0300455V, NSW/ACT)",                      MACHINE_NOT_WORKING, layout_dimtouch )        // 606, A - 06/03/97, Rev 14
GAMEL( 1997, magtcha5a,   magtcha5, aristmk5_touch,     dimtouch,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Magic Touch (0200455V, NSW/ACT)",                      MACHINE_NOT_WORKING, layout_dimtouch )        // 606, A - 06/03/97, Rev 14
GAMEL( 1997, mammothm,    aristmk5, aristmk5,           kgalah,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Mammoth Money (0100425V, NSW/ACT)",                    0,                   layout_kgalah )          // 595/5, D - 07/04/97, Rev 1.22.14.1
GAMEL( 2000, marmagic,    aristmk5, aristmk5,           goldenra,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Margarita Magic (01J00101, NSW/ACT)",                  MACHINE_NOT_WORKING, layout_marmagic )        // JB005, A - 07/07/00, Rev 17
GAMEL( 2000, marmagicu,   marmagic, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Margarita Magic (EHG1558, US)",                        MACHINE_NOT_WORKING, layout_marmagicu )       // US003, 07/07/2000
GAMEL( 2000, marmagicua,  marmagic, aristmk5_usa,       bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Margarita Magic (EHG1559, US)",                        MACHINE_NOT_WORKING, layout_adonisu )         // US003, A - 07/07/00
GAMEL( 1997, mgarden,     aristmk5, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Magic Garden (AHG1211-99, US)",                        MACHINE_NOT_WORKING, layout_aristmk5_us )     // MV4033, B - 10/02/97
GAMEL( 1996, minemine,    aristmk5, aristmk5,           swhr2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Mine, Mine, Mine (0400115V, NSW/ACT)",                 0,                   layout_swhr2 )           // 559/2, D - 16/01/96, Rev 1.18.3.0
GAMEL( 1996, minemineu,   minemine, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Mine, Mine, Mine (VHG0416-99, US)",                    MACHINE_NOT_WORKING, layout_aristmk5_us )     // 559/2, E - 14/02/96
GAMEL( 1996, minemineua,  minemine, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Mine, Mine, Mine (NHG0416-99, US)",                    MACHINE_NOT_WORKING, layout_aristmk5_us )     // 559/2, E - 14/02/96
GAMEL( 1997, monmouse,    aristmk5, aristmk5,           cashcham,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Money Mouse (0400469V, NSW/ACT)",                      MACHINE_NOT_WORKING, layout_cashcham )        // 607/1, B - 08/04/97, Rev 6
GAMEL( 1997, monmousea,   monmouse, aristmk5,           cashcham,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Money Mouse (0300469V, NSW/ACT)",                      MACHINE_NOT_WORKING, layout_cashcham )        // 607/1, B - 08/04/97, Rev 6
GAMEL( 2001, montree,     aristmk5, aristmk5,           montree,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Money Tree (0201397V, New Zealand)",                   0,                   layout_montree )         // MV4126, C - 12/04/01, Rev 5
GAMEL( 1996, mountmon,    aristmk5, aristmk5,           mountmon,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Mountain Money (0100294V, NSW/ACT)",                   0,                   layout_mountmon )        // 595/3, B - 11/06/96, Rev 1.22.5.0
GAMEL( 1996, mountmona,   mountmon, aristmk5,           mystgard,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Mountain Money (0100289V, NSW/ACT)",                   0,                   layout_mystgard )        // 595/2, C - 11/06/96, Rev 1.22.5.0
GAMEL( 2001, mountmonua,  mountmon, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Mountain Money (BHG1464, US)",                         MACHINE_NOT_WORKING, layout_magimaska )       // MV4108/5, A - 10/03/01
GAMEL( 2000, multidrw,    aristmk5, aristmk5,           multidrw,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Multidraw - Free Games (0200956V, NSW/ACT)",           MACHINE_NOT_WORKING, layout_multidrw )        // 386/64, E - 08/05/00, Rev 17
GAMEL( 1996, mystgard,    aristmk5, aristmk5,           mystgard,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Mystic Garden (0100275V, NSW/ACT)",                    0,                   layout_mystgard )        // 595/1, B - 11/06/96, Rev 1.22
GAMEL( 2001, one4all,     aristmk5, aristmk5,           one4all,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "One For All (0101503V, New Zealand)",                  0,                   layout_one4all )         // MV4141, A - 28/05/01, Rev 11
GAMEL( 1999, orchidms,    aristmk5, aristmk5,           orchidms,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Orchid Mist (0200849V, NSW/ACT)",                      MACHINE_NOT_WORKING, layout_orchidms )        // 601/3, C - 03/02/99, Rev 8
GAMEL( 1999, orchidmsa,   orchidms, aristmk5,           orchidms,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Orchid Mist (0100849V, NSW/ACT)",                      MACHINE_NOT_WORKING, layout_orchidms )        // 601/3, C - 03/02/99, Rev 6
GAMEL( 2000, orchidmsnz,  orchidms, aristmk5,           orchidmsnz,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Orchid Mist (0101241V, New Zealand)",                  0,                   layout_orchidmsnz )      // MV4118, A - 3/7/00, Rev 30
GAMEL( 1996, oscara5,     aristmk5, aristmk5,           aristmk5_9,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Oscar (0200348V, NSW/ACT)",                            MACHINE_NOT_WORKING, layout_dolphntrb )       // 593/2, C - 20/09/96, Rev 3
GAMEL( 1996, oscara5a,    oscara5,  aristmk5,           aristmk5_9,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Oscar (0100348V, NSW/ACT)",                            0,                   layout_dolphntrb )       // 593/2, C - 20/09/96, Rev 1.23.9.1
GAMEL( 1999, pantmag,     aristmk5, aristmk5,           kgalah,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Panther Magic (0101046V, NSW/ACT)",                    MACHINE_NOT_WORKING, layout_pantmag )         // 594/7, A - 06/10/99, Rev 16
GAMEL( 1999, pantmaga,    pantmag,  aristmk5,           pantmaga,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Panther Magic (0100716V, NSW/ACT)",                    MACHINE_NOT_WORKING, layout_pantmaga )        // 594/4, A - 13/05/98, Rev 9
GAMEL( 2001, partygrs,    aristmk5, aristmk5_usa_touch, aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Party Gras (AHG1567, US)",                             MACHINE_NOT_WORKING, layout_magimaska )       // MV4115/6, A - 10/11/01
GAMEL( 2001, partygrsa,   partygrs, aristmk5_usa_touch, bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Party Gras (BHG1284, US)",                             MACHINE_NOT_WORKING, layout_bparty )          // MV4115/3, B - 06/02/01
GAMEL( 2000, peaflut,     aristmk5, aristmk5,           trstrove,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Peacock Flutter (02J00011, NSW/ACT)",                  MACHINE_NOT_WORKING, layout_trstrove )        // JB001, A - 10/03/00, Rev 8
GAMEL( 1997, pengpay,     aristmk5, aristmk5,           cashchama,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Penguin Pays (0200460V, NSW/ACT)",                     MACHINE_NOT_WORKING, layout_cashchama )       // 586/4(a), D - 03/06/97, Rev 3
GAMEL( 1996, pengpaya,    pengpay,  aristmk5,           cashchama,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Penguin Pays (0200357V, NSW/ACT)",                     0,                   layout_cashchama )       // 586/4, C - 12/11/96
GAMEL( 1997, pengpayb,    pengpay,  aristmk5,           swhr2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Penguin Pays (0200359V, NSW/ACT)",                     MACHINE_NOT_WORKING, layout_swhr2 )           // 586/3(a), D - 03/06/97, Rev 4
GAMEL( 1995, pengpayc,    pengpay,  aristmk5,           wcougar,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Penguin Pays (0200113V, NSW/ACT)",                     0,                   layout_wcougar )         // 586, A - 12/10/95
GAMEL( 1997, pengpayu,    pengpay,  aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Penguin Pays (BHI0417-03, US)",                        MACHINE_NOT_WORKING, layout_aristmk5_us )     // 586/7(b), B - 14/07/97
GAMEL( 1997, pengpayua,   pengpay,  aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Penguin Pays (OHG0417-03, US)",                        MACHINE_NOT_WORKING, layout_aristmk5_us )     // 586/7(b), B - 14/07/97
GAMEL( 1997, pengpayub,   pengpay,  aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Penguin Pays (OHG0417-02, US)",                        MACHINE_NOT_WORKING, layout_aristmk5_us )     // 586/7(b), B - 14/07/97
GAMEL( 2001, pengpayce,   pengpay,  aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Penguin Pays - Cash Express (AHG1544, US)",            MACHINE_NOT_WORKING, layout_pengpayce )       // MV4122,  C - 19/01/01
GAMEL( 2001, pengpuck,    pengpay,  aristmk5_usa,       pengpuck,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Penguin Pays - Penguin Pucks (EHG1257, US)",           MACHINE_NOT_WORKING, layout_pengpuck )        // MV4122/1, C - 19/01/01
GAMEL( 1998, penpir,      aristmk5, aristmk5,           kgalah,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Penguin Pirate (0100674V, NSW/ACT)",                   MACHINE_NOT_WORKING, layout_kgalah )          // 619/1, A - 31/03/98, Rev 10
GAMEL( 1998, penpira,     penpir,   aristmk5,           snowcat,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Penguin Pirate (0200578V, NSW/ACT)",                   MACHINE_NOT_WORKING, layout_snowcat )         // 619, A - 27/02/98, Rev 8
GAMEL( 1998, petshop,     aristmk5, aristmk5,           petshop,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Pet Shop (0100731V, NSW/ACT)",                         MACHINE_NOT_WORKING, layout_petshop )         // 618/1, A - 17/04/98, Rev 13
GAMEL( 1995, phantpay,    aristmk5, aristmk5,           swhr2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Phantom Pays (0500005V, NSW/ACT)",                     0,                   layout_swhr2 )           // 570/1, E - 12/09/95
GAMEL( 1996, przfight,    aristmk5, aristmk5,           przfight,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Prize Fight (0100299V, NSW/ACT)",                      0,                   layout_przfight )        // 578/4, B - 08/08/96
GAMEL( 1998, qcash,       aristmk5, aristmk5,           kgalah,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queens of Cash (0100706V, NSW/ACT)",                   MACHINE_NOT_WORKING, layout_kgalah )          // 603/6, C  - 23/07/98, Rev 6
GAMEL( 1997, qnile,       aristmk5, aristmk5,           qnile,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile (0300439V, NSW/ACT)",                MACHINE_NOT_WORKING, layout_qnile )           // 602/4, B - 13/05/97, Rev 7
GAMEL( 1997, qnilea,      qnile,    aristmk5,           qnile,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile (0200439V, NSW/ACT)",                MACHINE_NOT_WORKING, layout_qnile )           // 602/4, B - 13/05/97, Rev 7
GAMEL( 1997, qnileb,      qnile,    aristmk5,           qnile,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile (0100439V, NSW/ACT)",                0,                   layout_qnile )           // 602/4, B - 13/05/97, Rev 1.26.18.1
GAMEL( 2002, qnilebr,     qnile,    aristmk5,           goldpyrb,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile (0101707V, Brazil)",                 0,                   layout_goldpyrb )        // MV4162, A - 21/08/02
GAMEL( 1997, qnilec,      qnile,    aristmk5,           qnilec,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile (0300440V, NSW/ACT)",                MACHINE_NOT_WORKING, layout_qnilec )          // 602/3, B - 13/05/97, Rev 5
GAMEL( 1999, qniled,      qnile,    aristmk5,           checkma5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile (0101139V, NSW/ACT)",                MACHINE_NOT_WORKING, layout_qniled )          // 602/16, A - 11/10/99, Rev 14
GAMEL( 2000, qnilenl,     qnile,    aristmk5,           qnilenl,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile (0301059V, Holland)",                0,                   layout_qnilenl )         // 602/5, G - 10/04/00
GAMEL( 1998, qnilenz,     qnile,    aristmk5,           cashcatnz,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile (0300785V, New Zealand)",            0,                   layout_cashcatnz )       // MV4068, A - 31/8/98, Rev 20
GAMEL( 1997, qnileu,      qnile,    aristmk5_usa,       dolphntru,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile (GHG4091-02, US)",                   MACHINE_NOT_WORKING, layout_aristmk5_us )     // MV4091, B - 13/05/97
GAMEL( 1997, qnileua,     qnile,    aristmk5_usa,       dolphntru,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile (GHG4091-03, US)",                   MACHINE_NOT_WORKING, layout_aristmk5_us )     // MV4091, B - 13/05/97
GAMEL( 1997, qnilev,      qnile,    aristmk5,           aristmk5_9,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile (04J00784, Venezuela)",              MACHINE_NOT_WORKING, layout_dolphntrb )       // 602/3, B - 13/05/97, Rev 6
GAMEL( 2001, qnilece,     qnile,    aristmk5_usa,       dolphntrce,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile - Cash Express (AHG1609, US)",       MACHINE_NOT_WORKING, layout_adonisu )         // MV4091/1, A - 17/01/01
GAMEL( 2001, qnilecea,    qnile,    aristmk5_usa,       dolphntru,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile - Cash Express (AHG1525, US)",       MACHINE_NOT_WORKING, layout_qnilecea )        // MV4091, F - 17/01/01
GAMEL( 2001, qnileceb,    qnile,    aristmk5_usa,       dolphntru,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile - Cash Express (AHG1608, US)",       MACHINE_NOT_WORKING, layout_magimaska )       // MV4091, F - 17/01/01
GAMEL( 1999, qnilemax,    qnile,    aristmk5_touch,     trstrove,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile - Maximillions (0401072V, NSW/ACT)", MACHINE_NOT_WORKING, layout_trstrove )        // 602/4, D - 18/06/99, Rev 14
GAMEL( 1994, qtbird,      aristmk5, aristmk5,           qtbird,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Q.T. Bird (0500009V, NSW/ACT)",                        0,                   layout_qtbird )          // 581, A - 27/10/94, Rev 1.1.1.0
GAMEL( 2000, rainwrce,    aristmk5, aristmk5,           adonisce,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Rainbow Warriors - Cash Express (0101332V, NSW/ACT)",  MACHINE_NOT_WORKING, layout_adonisce )        // 655, B - 02/03/00, Rev 25
GAMEL( 1998, reelpwr,     aristmk5, aristmk5,           wizways,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Reel Power (0100400V, NSW/ACT)",                       0,                   layout_wizways )         // 598/2, A - 01/11/96, Rev 1.23.8.0
GAMEL( 1998, reelrock,    aristmk5, aristmk5,           reelrock,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Reelin-n-Rockin (0100779V, NSW/ACT)",                  MACHINE_NOT_WORKING, layout_reelrock )        // 628, A - 13/07/98, Rev 5
GAMEL( 1997, retrsam,     aristmk5, aristmk5,           retrsam,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Return of the Samurai (0400549V, NSW/ACT)",            MACHINE_NOT_WORKING, layout_retrsam )         // 608, A - 17/04/97, Rev 5
GAMEL( 1997, retrsama,    retrsam,  aristmk5,           retrsam,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Return of the Samurai (0200549V, NSW/ACT)",            0,                   layout_retrsam )         // 608, A - 17/04/97, Rev 1.26.25.3
GAMEL( 1997, retrsamb,    retrsam,  aristmk5,           retrsamb,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Return of the Samurai (0200506V, NSW/ACT)",            0,                   layout_retrsamb )        // 608, A - 17/04/97, Rev 1.26.25.3
GAMEL( 1997, rushrst,     aristmk5, aristmk5,           rushrst,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Rushin Rooster (0100534V, NSW/ACT)",                   0,                   layout_cashchama )       // 596/3, C - 25/06/97, Rev 1.27.2.3
GAMEL( 2001, sldeluxe,    aristmk5, aristmk5_usa,       bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Sweet Liberty Deluxe (AHG1575, US)",                   MACHINE_NOT_WORKING, layout_adonisu )         // MV4137, A - 11/02/01
GAMEL( 1998, slvrwolf,    aristmk5, aristmk5,           wamazona,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Silver Wolf (0100673V, NSW/ACT)",                      MACHINE_NOT_WORKING, layout_wamazona )        // 621/2, A - 23/03/98, Rev 3
GAMEL( 1996, snowcat,     aristmk5, aristmk5,           snowcat,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Snow Cat (0100405V, NSW/ACT)",                         0,                   layout_snowcat )         // 599, B - 23/12/96, Rev 1.25.1.0
GAMEL( 1997, sumospin,    aristmk5, aristmk5,           swhr2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Sumo Spins (0200606V, NSW/ACT)",                       MACHINE_NOT_WORKING, layout_swhr2 )           // 622, A - 08/12/97, Rev 4
GAMEL( 1998, sbuk3,       aristmk5, aristmk5,           sbuk3,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Super Bucks III (0200711V, NSW/ACT)",                  MACHINE_NOT_WORKING, layout_sbuk3 )           // 626, A - 22/04/98, Rev 8
GAMEL( 1998, sbuk3a,      sbuk3,    aristmk5,           sbuk3,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Super Bucks III (0100711V, NSW/ACT)",                  MACHINE_NOT_WORKING, layout_sbuk3 )           // 626, A - 22/04/98, Rev 7
GAMEL( 1995, swhr2,       aristmk5, aristmk5,           swhr2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Sweethearts II (0200465V, NSW/ACT)",                   MACHINE_NOT_WORKING, layout_swhr2 )           // 577/1, C - 07/09/95, Rev 3
GAMEL( 1995, swhr2a,      swhr2,    aristmk5,           swhr2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Sweethearts II (0200004V, NSW/ACT)",                   0,                   layout_swhr2 )           // 577/1, C - 07/09/95, Rev 1.1.3.0
GAMEL( 1998, swhr2u,      swhr2,    aristmk5_usa,       swhr2u,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Sweethearts II (PHG0742-02, US)",                      MACHINE_NOT_WORKING, layout_swhr2u )          // MV4061, A - 29/06/98
GAMEL( 1995, swhr2v,      swhr2,    aristmk5,           swhr2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Sweethearts II (01J01986, Venezuela)",                 MACHINE_NOT_WORKING, layout_swhr2 )           // 577/1, C - 07/09/95, Rev 3
GAMEL( 1996, thor,        aristmk5, aristmk5,           cashcham,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Thor (0200319V, NSW/ACT)",                             0,                   layout_cashcham )        // 569/12, B - 14/08/96, Rev 1.23.7.0
GAMEL( 1996, thndh,       aristmk5, aristmk5,           snowcat,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Thunder Heart (0200333V, NSW/ACT)",                    0,                   layout_snowcat )         // 570/9, A - 14/08/96, Rev 1.23.6.0
GAMEL( 1996, thndha,      thndh,    aristmk5,           wildbill,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Thunder Heart (0200334V, NSW/ACT)",                    0,                   layout_wildbill )        // 597/1, A - 14/08/96, Rev 1.23.6.0
GAMEL( 1997, topbana,     aristmk5, aristmk5,           wildbill,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Top Banana (0100550V, NSW/ACT)",                       0,                   layout_wildbill )        // 594/3, A - 18/08/97, Rev 1.26.39.2
GAMEL( 1998, toutango,    aristmk5, aristmk5,           kgalah,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Toucan Tango (0100782V, NSW/ACT)",                     MACHINE_NOT_WORKING, layout_kgalah )          // 616/1, A - 17/06/98, Rev 16
GAMEL( 1999, toutangonl,  toutango, aristmk5,           toutangonl,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Toucan Tango (0301388V, Holland)",                     0,                   layout_toutangonl )      // 616, C - 11/05/99
GAMEL( 2000, trstrove,    aristmk5, aristmk5,           trstrove,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Treasure Trove (01J00161, NSW/ACT)",                   MACHINE_NOT_WORKING, layout_trstrove )        // JB001/3, A - 5/10/00, Rev 8
GAMEL( 2002, tritreat,    aristmk5, aristmk5,           trstrove,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Triple Treat (0201692V, NSW/ACT)",                     MACHINE_NOT_WORKING, layout_tritreat )        // 692, A - 17/05/02, Rev 25
GAMEL( 2001, trojhors,    aristmk5, aristmk5,           goldenra,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Trojan Horse (01J00851, NSW/ACT)",                     MACHINE_NOT_WORKING, layout_marmagic )        // JB001/5, A - 30/10/01, Rev 17
GAMEL( 1996, trpdlght,    aristmk5, aristmk5,           swhr2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Tropical Delight (0100269V, NSW/ACT)",                 0,                   layout_swhr2 )           // 577/3, B - 15/05/96, Rev 1.22.2.0
GAMEL( 1997, trpdlghtu,   trpdlght, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Tropical Delight (PHG0625-02, US)",                    MACHINE_NOT_WORKING, layout_aristmk5_us )     // 577/3, D - 24/09/97
GAMEL( 1998, unicornd,    aristmk5, aristmk5,           aristmk5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Unicorn Dreaming (0100791V, NSW/ACT)",                 MACHINE_NOT_WORKING, layout_aristmk5 )        // 631/1 A, A - 31/08/98, Rev 12
GAMEL( 1998, unicornda,   unicornd, aristmk5,           aristmk5_9,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Unicorn Dreaming (0100813V, NSW/ACT)",                 MACHINE_NOT_WORKING, layout_dolphntrb )       // 631 A, A - 02/09/98, Rev 14
GAMEL( 2000, unicorndnz,  unicornd, aristmk5,           unicorndnz,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Unicorn Dreaming (0101228V, New Zealand)",             0,                   layout_aristmk5 )        // MV4113/1, A - 05/04/2000, Rev 27
GAMEL( 2001, unicorndu,   unicornd, aristmk5_usa,       bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Unicorn Dreaming (BHG1584, US)",                       MACHINE_NOT_WORKING, layout_adonisu )         // MV4130/1, C - 10/17/01
GAMEL( 2002, venicea5,    aristmk5, aristmk5,           goldenra,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Venice (02J02056, Venezuela)",                         MACHINE_NOT_WORKING, layout_venicea5 )        // JB009/2, B - 11/07/02, Rev 17
GAMEL( 2001, wafricau,    aristmk5, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wild Africa (AHG1535, US)",                            MACHINE_NOT_WORKING, layout_magimaska )       // MV4076, B - 17/07/01
GAMEL( 1996, wamazon,     aristmk5, aristmk5,           wamazon,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wild Amazon (0200507V, NSW/ACT)",                      MACHINE_NOT_WORKING, layout_wamazon )         // 506/8, A - 10/10/96, Rev 3
GAMEL( 1996, wamazona,    wamazon,  aristmk5,           wamazona,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wild Amazon (0200285V, NSW/ACT)",                      0,                   layout_wamazona )        // 506/6, A - 7/5/96, Rev 1.22.8.0
GAMEL( 1996, wamazonv,    wamazon,  aristmk5,           wamazon,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wild Amazon (01J01996, Venezuela)",                    MACHINE_NOT_WORKING, layout_wamazon )         // 506/8, A - 10/10/96, Rev 3
GAMEL( 1997, wikwin,      aristmk5, aristmk5,           wikwin,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wicked Winnings (0100553V, NSW/ACT)",                  MACHINE_NOT_WORKING, layout_wikwin )          // 609, B - 01/07/97, Rev 1
GAMEL( 1996, wldangel,    aristmk5, aristmk5,           swhr2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wild Angels (0100337V, NSW/ACT)",                      0,                   layout_swhr2 )           // 600, B - 24/09/96
GAMEL( 1996, wildbill,    aristmk5, aristmk5,           wildbill,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wild Bill (0100297V, NSW/ACT)",                        0,                   layout_wildbill )        // 543/8, C - 15/08/96, Rev 1.22.12.0
GAMEL( 1996, wcougar,     aristmk5, aristmk5,           wcougar,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wild Cougar (0100167V, NSW/ACT)",                      0,                   layout_wcougar )         // 569/9, B - 27/2/96, Rev 1.18.1.0
GAMEL( 1997, wcougaru,    wcougar,  aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wild Cougar (NHG0296-04, US)",                         MACHINE_NOT_WORKING, layout_aristmk5_us )     // 569/8, D - 19/05/97
GAMEL( 1997, wcougarua,   wcougar,  aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wild Cougar (NHG0296-07, US)",                         MACHINE_NOT_WORKING, layout_aristmk5_us )     // 569/8, D - 19/05/97
GAMEL( 1997, wcougarub,   wcougar,  aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wild Cougar (NHG0296-99, US)",                         MACHINE_NOT_WORKING, layout_aristmk5_us )     // 569/8, D - 19/05/97
GAMEL( 1997, wcougaruc,   wcougar,  aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wild Cougar (AHG1531, US)",                            MACHINE_NOT_WORKING, layout_magimaska )       // MV4055, B - 13/05/97
GAMEL( 1996, wizways,     aristmk5, aristmk5,           wizways,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wizard Ways (0200396V, NSW/ACT)",                      MACHINE_NOT_WORKING, layout_wizways )         // 598/3, A - 04/11/96, Rev 9
GAMEL( 1997, wnpost,      aristmk5, aristmk5_usa,       wnpost,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Winning Post (RHG0418-04, US)",                        MACHINE_NOT_WORKING, layout_wnpost )          // 541/2, G - 11/02/97
GAMEL( 1999, wthing,      aristmk5, aristmk5,           retrsam,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wild Thing (0101158V, NSW/ACT)",                       MACHINE_NOT_WORKING, layout_retrsam )         // 608/4, B - 14/12/99, Rev 8
GAMEL( 1999, wtiger,      aristmk5, aristmk5,           wtiger,       aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "White Tiger Classic (0200954V, NSW/ACT)",              MACHINE_NOT_WORKING, layout_wtiger )          // 638/1, B - 08/07/99, Rev 13
GAMEL( 2000, yukongl5,    aristmk5, aristmk5,           goldenra,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Yukon Gold (03J00191, NSW/ACT)",                       MACHINE_NOT_WORKING, layout_yukongl5 )        // JB005/1, A - 30/10/2000, Rev 17

// the following parent sets boot but cannot be played due to unemulated comms
GAMEL( 1996, jungjuic,    aristmk5, aristmk5,           jungjuic,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Jungle Juice (0200240V, New Zealand)",                 MACHINE_NOT_WORKING, layout_jungjuic )        // 566/3, F - 06/03/96
GAMEL( 1998, penpir2,     aristmk5, aristmk5,           penpir2,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Penguin Pirate II (0100869V, Victoria)",               MACHINE_NOT_WORKING, layout_penpir2 )         // 619/3, A - 17/12/98

// the following clone sets boot but cannot be played due to unemulated comms
GAMEL( 1996, bumblbugql,  bumblbug, aristmk5,           swhr2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Bumble Bugs (0200456V, Queensland)",                   MACHINE_NOT_WORKING, layout_swhr2 )           // 593, D - 5/07/96, Rev 1.0
GAMEL( 1998, chickna5ql,  chickna5, aristmk5,           chickna5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Chicken (0200530V, Queensland)",                       MACHINE_NOT_WORKING, layout_snowcat )         // 596, C - 23/02/98, Rev 1.0
GAMEL( 2000, dolphntrql,  dolphntr, aristmk5,           trstrove,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Dolphin Treasure (0101250V, Queensland)",              MACHINE_NOT_WORKING, layout_trstrove )        // 602/17, A - 20/01/00, Rev 1.0.1.0
GAMEL( 1998, goldpyrb,    goldpyr,  aristmk5,           goldpyrb,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Golden Pyramids (0100878V, Victoria)",                 MACHINE_NOT_WORKING, layout_goldpyrb )        // 602/5, C - 19/06/98
GAMEL( 2000, indrema5ql,  indrema5, aristmk5,           aristmk5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Indian Dreaming (0101340V, Queensland)",               MACHINE_NOT_WORKING, layout_aristmk5 )        // 628/3, A - 02/05/00, Rev 1.0
GAMEL( 1999, qnileql,     qnile,    aristmk5,           trstrove,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile (0201200V, Queensland)",             MACHINE_NOT_WORKING, layout_trstrove )        // 602/4, D - 18/06/99
GAMEL( 2001, qnilefl,     qnile,    aristmk5,           trstrove,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Queen of the Nile - Fast Lane (0101670V, Queensland)", MACHINE_NOT_WORKING, layout_trstrove )        // 602/4, E - 17/08/01

// the following parent sets are known bad dumps, and do not boot (confirmed)
GAMEL( 1996, blackpnt,    aristmk5, aristmk5,           wildbill,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Black Panther (0200818V, Victoria)",                   MACHINE_NOT_WORKING, layout_wildbill )        // 594/1, A - 30/07/96
GAMEL( 1996, canrose,     aristmk5, aristmk5_usa,       bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Canyon Rose (AHG1463, US)",                            MACHINE_NOT_WORKING, layout_cashchamu )       // 603(a), B - 06/12/96 (same as Cash Chameleon)
GAMEL( 2000, diamdest,    aristmk5, aristmk5_usa,       bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Diamond Destiny (AHG1533, US)",                        MACHINE_NOT_WORKING, layout_aristmk5_us_200 ) // MV4115_5, A - 09/05/2000 (same as Magic Mask)
GAMEL( 2001, fortfvr,     aristmk5, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Fortune Fever (BHG1566, US)",                          MACHINE_NOT_WORKING, layout_aristmk5_us )     // MV4122/2, A - 13/05/01
GAMEL( 1996, jumpbean,    aristmk5, aristmk5,           swhr2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Jumping Beans (0100161V, NSW/ACT)",                    MACHINE_NOT_WORKING, layout_swhr2 )           // 586/2, A - 25/01/96
GAMEL( 1999, sbuk2,       aristmk5, aristmk5,           sbuk2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Super Bucks II (0400501V, NSW/ACT)",                   MACHINE_NOT_WORKING, layout_sbuk2 )           // 578, G - 26/07/99, Rev 7
GAMEL( 2001, wcoyote,     aristmk5, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wild Coyote (AHG1515, US)",                            MACHINE_NOT_WORKING, layout_aristmk5_us )     // MV4134, A - 30/07/01 (same as Loco Loot)

// the following clone sets are known bad dumps, and do not boot (confirmed)
GAMEL( 2000, bpartya,     bparty,   aristmk5_usa_touch, bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Bachelorette Party (BHG1579, US)",                     MACHINE_NOT_WORKING, layout_bparty )          // MV4119/1, B - 25/08/2000
GAMEL( 1997, cashcra5a,   cashcra5, aristmk5,           aristmk5_9,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Cash Crop (0300447V, NSW/ACT)",                        MACHINE_NOT_WORKING, layout_dolphntrb )       // 607/2, C - 29/08/97, Rev 7
GAMEL( 1996, chickna5v,   chickna5, aristmk5,           chickna5v,    aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Chicken (01J01886, Venezuela)",                        MACHINE_NOT_WORKING, layout_chickna5v )       // 596/1, B - 23/12/96, Rev 3
GAMEL( 2001, dynajacku,   dynajack, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Dynamite Jack (CHG1562, US)",                          MACHINE_NOT_WORKING, layout_magimaska )       // US002, A - 11/07/01
GAMEL( 2000, eforsta5ce,  eforsta5, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Enchanted Forest - Cash Express (CHG1536, US)",        MACHINE_NOT_WORKING, layout_aristmk5_us )     // MV4108/6, C - 17/01/00
GAMEL( 2001, mountmonce,  mountmon, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Mountain Money - Cash Express (AHG1629, US)",          MACHINE_NOT_WORKING, layout_aristmk5_us )     // MV4108/5, A - 10/03/01
GAMEL( 2001, mountmonu,   mountmon, aristmk5_usa,       aristmk5_usa, aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Mountain Money (BHG1465, US)",                         MACHINE_NOT_WORKING, layout_aristmk5_us )     // MV4108/5, A - 10/03/01
GAMEL( 2001, partygrsb,   partygrs, aristmk5_usa_touch, bootsctnua,   aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Party Gras (AHG1568, US)",                             MACHINE_NOT_WORKING, layout_bparty )          // MV4115/6, A - 10/11/2001, 20 lines
GAMEL( 1995, pengpayd,    pengpay,  aristmk5,           wcougar,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Penguin Pays (0300113V, NSW/ACT)",                     MACHINE_NOT_WORKING, layout_wcougar )         // 586, A - 12/10/95, Rev 4
GAMEL( 1998, petshopa,    petshop,  aristmk5,           snowcat,      aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Pet Shop (0100679V, NSW/ACT)",                         MACHINE_NOT_WORKING, layout_snowcat )         // 618, A - 09/03/98, Rev 10
GAMEL( 2000, reelrockql,  reelrock, aristmk5,           trstrove,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Reelin-n-Rockin (0101460V, Queensland)",               MACHINE_NOT_WORKING, layout_trstrove )        // 628/2, E - 20/12/00
GAMEL( 1995, sbuk2a,      sbuk2,    aristmk5,           sbuk2,        aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Super Bucks II (0300006V, NSW/ACT)",                   MACHINE_NOT_WORKING, layout_sbuk2 )           // no data due to missing ROMs
GAMEL( 2000, wthinga,     wthing,   aristmk5,           aristmk5,     aristmk5_state, init_aristmk5, ROT0, "Aristocrat", "Wild Thing (0201176V, NSW/ACT)",                       MACHINE_NOT_WORKING, layout_aristmk5 )        // 608/5, B - 25/02/00, Rev 11
