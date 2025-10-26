## Student Information
- Name: Christian Wilkins
- Email: cwilkin2@umbc.edu/va96371@umbc.edu
- Student ID: VA96371
- Class Section: 04-LEC (1410)

## Project Description
This project is a multilevel priority queue implemented in the kernel. The user can push and pull nodes from it and it is threadsafe. 
There are various tests to showcase robustness against deadlocks, memory leaks, and multithreadding race conditions. It implements 4 syscalls:
init kern application, free kern application, kern add priority, and kern get priority to expose the queues to the user_space. 
It is a newly compiled kernel with the calls accessible.

## How to Configure, Compile, and Install the Custom Kernel
1. Make new folder to place files (mkdir /usr/src/project3)
2. Run git clone git@github.com:UMBC-CMSC421/project3-fa25-Cawwilkins.git
3. Set Extraversion in Makefile at root of repository
4. If havent already, run make mrproper and make xconfig
5. Run curl https://userpages.cs.umbc.edu/lsebald1/cmsc421-fa2022/dotconfig > ./.config, to speed up process
6. Run make bindeb-pkg -j$(nproc)
7. Go up one folder
8. Run Sudo dpkg -i {highestNumberCustomDebFile}.deb
9. Restart machine and select kernel version with earlier set Extraversion
10. Kernel is now installed!

## How to Compile and Run the Proof-of-Concept Userspace Program
1. If havent already, run make mrproper and make xconfig
2. Run curl https://userpages.cs.umbc.edu/lsebald1/cmsc421-fa2022/dotconfig > ./.config, to speed up process
3. Run make bindeb-pkg -j$(nproc)
4. Go up one folder
5. Run Sudo dpkg -i {highestNumberCustomDebFile}.deb
6. Restart machine and select kernel version with earlier set Extraversion
7. Navigate to root of repository, open code tests, and run make build_poc
8. Run make run to run the proof of concept

## How to Compile and Run the Testing Suite Userspace Program
1. If havent already, run make mrproper and make xconfig
2. Run curl https://userpages.cs.umbc.edu/lsebald1/cmsc421-fa2022/dotconfig > ./.config, to speed up process
3. Run make bindeb-pkg -j$(nproc)
4. Go up one folder
5. Run Sudo dpkg -i {highestNumberCustomDebFile}.deb
6. Restart machine and select kernel version with earlier set Extraversion
7. Navigate to root of repository, open code tests, and run make build_test
8. Run make run_t to run test suite

## Known Project Issues
1. None known

## LLM/AI Prompts Used
1. When I run my kernel mem check, I am getting no output but it is on my first attempt so I am doubtful I dont have any memory leaks, is
there some other place I should be looking?: I got no output on my first memory check leak so I thought I was either doing something wrong or
looking in the wrong place but as far as I know there were actually just no memory leaks.
2. What is the syntax for printing a string in C?: I couldnt remember which letter was for strings w the % to print my data for a node
3. What is the difference between destroying and kfreeing a lock?: I saw people talking about destroying locks in the discord but I had only
kfreed mine so I wanted to know what the difference was so I could figure out if I needed to do it. 
4. What is the difference between pthreads and forking?: I was trying to figure out if I needed to use forking in this project but as far as 
I could tell, I didn't need to. 

## Sources Used
1. https://www.man7.org/linux/man-pages/man3.org/linux/man-pages/man3/pthread_create.3.html
2. https://www.geeksforgeeks.org/c/multithreading-in-c/
3. https://www.geeksforgeeks.org/cpp/how-to-create-a-thread-in-cpp/
4. https://www.geeksforgeeks.org/c/c-typecasting/
5. https://www.squash.io/how-to-fix-undefined-reference-to-pthread_create-in-linux/
6. https://www.kernel.org/doc/html/latest/dev-tools/kmemleak.html
