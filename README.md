# CIS 452 Bake Off Project

## Project Description
For this project you will be using semaphores, threads and shared memory.
Each baker will run as its own thread. Access to each resource will require a counting/binary
semaphore as appropriate.
The program should prompt the user for a number of bakers. Each baker will be competing for
the resources to create each of the recipes.
Each baker is in a shared kitchen with the following resources:
- Mixer - 2
- Pantry – 1
- Refrigerator - 2
- Bowl – 3
- Spoon – 5
- Oven – 1

Ingredients available in the pantry:
1. Flour
2. Sugar
3. Yeast
4. Baking Soda
5. Salt
6. Cinnamon

Ingredients available in the refrigerator:
1. Egg(s)
2. Milk
3. Butter

Only one baker may be in the pantry at a time.
Only one baker may be in the refrigerator at a time.

To ‘bake’ a recipe, the baker must acquire each of the ingredients listed below:
- Cookies: Flour, Sugar, Milk, Butter
- Pancakes: Flour, Sugar, Baking soda, Salt, Egg, Milk, Butter
- Homemade pizza dough: Yeast, sugar, salt
- Soft Pretzels: Flour, sugar, salt, yeast, baking soda, Egg
- Cinnamon rolls: Flour, sugar, salt, butter, eggs, cinnamon

Once all of the ingredients have been acquired a bowl, a spoon and a mixer must be acquired to
mix the ingredients together. After they have been mixed together, it must be cooked in the oven.
Each baker must complete (and cook) each of the recipes once and then announce they have finished.

## Hand In
1. A design document clearly describing your project as well as the implementation
2. The source code in c (no zip files please)
3. Screenshot(s) of the execution which clearly shows what each of the bakers are doing in
real time (please limit your execution submission to 3 bakers to reduce the amount of
output). The screenshots should be from a terminal where the output is easy to read
(e.g., not a screenshot of visual studio code executing the program).

## Grading
Points will be deducted based on features that are missing in the project. The more
prominent the feature, the larger number of points. The project should avoid any deadlock
situations and use semaphores for any shared resources.

## Extra Credit
1. Turn the project in 2 weeks early (+10%)
2. Assign one of the bakers to have a chance to be ‘Ramsied’, this means they must release
all semaphores and start their current recipe from the beginning (+10%)
