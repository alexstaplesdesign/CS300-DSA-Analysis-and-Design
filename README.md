# CS 300 DSA: Analysis and Design

This repository holds my work from both CS 300 projects. Everything comes from the same scenario, an advising program for ABCU's Computer Science department.

- **CS300 Milestone One.docx** has my pseudocode for loading the course file, building course objects, and searching and printing a course with a vector.
- **CS300 Milestone Two.docx** has the same pseudocode built around a hash table.
- **CS300 Milestone Three.docx** has the same pseudocode built around a binary search tree.
- **CS300 Project One Pseudocode and Runtime Analysis.docx** is my final Project One document. It has the menu and sorted course list pseudocode, a Big O runtime and memory analysis of each structure, and my recommendation for which structure the program should use.
- **ProjectTwo.cpp** is the working C++ program from Project Two. It loads the course file into a hash table, prints every Computer Science course in alphanumeric order, and prints the title and prerequisites for any single course.
- **CS 300 ABCU_Advising_Program_Input.csv** is the course file the program reads.

## Reflection

### What was the problem you were solving in the projects for this course?

ABCU's advisors needed a program that reads a file of courses and prerequisites and answers two questions. The first is "what are all the courses, in order?" The second is "what's this one course and what do I need to take before it?" Project One was about designing that program and picking the right data structure for it. Project Two was building it.

### How did you approach the problem? Consider why data structures are important to understand.

I started by writing pseudocode for all three structures we covered and then costing out the file loading line by line. From there, I had to make a decision. Each structure is good at one of the two requests and weaker at the other. A binary search tree keeps courses sorted for free, but a hash table finds a single course in O(1) on average. I chose  the hash table  i figure advisors would be looking up one course more often than the whole list. Sorting a copy for the full list costs O(n log n), and that only happens when someone asks for it. Without understanding how each structure behaves, I would have picked whatever sounded best and never seen the trade-off.

### How did you overcome any roadblocks you encountered while going through the activities or project?

I kept thinking that the tree would be the best fit. At first glance it looked like it would be, so I traced the ABCU file through it by hand to check. The file is almost sorted already, so every course ended up as the only child of the one before it. I got a chain eight levels deep, and searching that would be no better than a vector. Tracing it with the real data is what changed my recommendation.

The other roadblocks were smaller file problems. A course can list a prerequisite that shows up later in the file, so I moved prerequisite checking until after everything loaded. The file also had Windows line endings, and the hidden \r at the end of each line broke my string matches until I trimmed it off every field.

### How has your work on this project expanded your approach to designing software and developing programs?

I used to jump straight into code. This course got me writing pseudocode first and thinking about runtime before I commit to anything. Writing the design first also made the C++ go faster, because I already had a plan to translate. I also learned to test a design against the data it'll get in practice.

### How has your work on this project evolved the way you write programs that are maintainable, readable, and adaptable?

I break programs into small functions that each do one job, with a one-line comment at the top saying what it does. Reusing functions helped a lot too. The same searchCourse handles duplicate checks, prerequisite validation, and printing a course, so a fix in one place fixes all three. I also handle bad input on purpose now, like blank fields, duplicate course numbers, lowercase course numbers, and menu choices that aren't valid. Swapping the hash table for another structure later would mostly mean just rewriting the load, search, and print functions while the menu stays the same.
