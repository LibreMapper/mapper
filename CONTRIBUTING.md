We are pleased by your interest in making LibreMapper a better software. There
are several areas where your contributions will matter. Please follow the
guidelines for the various areas.

Please mind that the guidelines are not perfect and will evolve over time.

# I've found a defect that should be fixed
Please report the issue at
[https://github.com/LibreMapper/mapper/issues](https://github.com/LibreMapper/mapper/issues).
Make effort to follow the issue template and provide enough information so that
a person who has no clue at the beginning what is the matter has some idea
about what is the trouble after reading your report.

If you are sharing data files as part of the issue report, please take care to
exclude sensitive and/or confidential information like new maps or personal
data. Remember that the files are visible to anyone in the issue tracking
system.

# I'm a coder and want to enhance the program
This news made our day! Depending on your preferences and skill level, you may
choose from the following options
* Pick a bug to squash in the [issue tracking system](https://github.com/LibreMapper/mapper/issues).
  Choose one, dive into the code, fix it.  Entry-level bugs are labeled as
  *help wanted*. Those are believed to be easier to fix than the others.
* Pick an enhancement to work on in the [active projects](https://github.com/LibreMapper/mapper/projects).
  These tasks may be substantially more complex than bug squashing because the
  demand not only coding skills but also software design skills.

The project strives to use so called Modern C++. Although you will see some
legacy bits in the code base, we take inspiration from the [C++ Core
Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) and
have a few additional [style rules in
place](https://github.com/OpenOrienteering/mapper/wiki/Coding-conventions)
(link to the OpenOrienteering Mapper page).

Patches are accepted through the [GitHub pull request
mechanism](https://github.com/LibreMapper/mapper/pulls). Alternatively, a patch
submission via e-mail to the LibreMapper maintainer, like in Linux and many
other FOSS projects, is acceptable. Please take care to structure your code
change in a patch series.

On the philosophy level we acknowledge the [KISS
principle](https://en.wikipedia.org/wiki/KISS_principle). Simple is better than
complex, dumb is better than ingenious. An average programmer should be able to
read the code fluently. Thick parts that require lots of effort to be
understood are undesirable, unless the problem itself is complex. When
complexity cannot be avoided though, support your future peers with comments
and concise commit messages.

Another principle we follow is expressed by the phrase [Perfect is the enemy of
good](https://en.wikipedia.org/wiki/Perfect_is_the_enemy_of_good).  It's enough
for a code change to be eligible for a merge that it alleviates a problem or
provides "mostly complete" implementation. When there is value in the
contribution or the code change does not make the code base more cluttered, the
patch is worth a submission.

# I've got a great innovative idea
Let us start with an excerpt from the book In Search of Excellence by T. Peters
and H. Watermann:

*Creativity is thinking up new things. Innovation is doing new things.  ... A
powerful new idea can kick around unused in a company for years, not because
its merits are not recognized, but because nobody has assumed the
responsibility for converting it from words into action. Ideas are useless
unless used. The proof of their value is only in their implementation. Until
then, they are in limbo.*

As explained by the management literature classics, an idea is the first step.
If you can implement your idea in the code, please attempt it.  A proof of
concept is a great starting point for a conversation. When a proof of concept
is not in the reach, illustrations help a lot.  In any case, please attempt to
explain what is the use of the change, and how LibreMapper users may benefit
from the change.
