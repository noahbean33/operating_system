# Exploration: Introduction to Operating Systems

## Introduction

An **Operating System (OS)** is a software program that sits between software applications and the computer hardware. All applications interact with the OS in some fashion. Relatively few of you are likely to work on the code of an OS itself. However, to be a good application developer you need to understand the capabilities of the OS and how best to utilize them to write applications that perform well and are secure.

The OS provides convenient **abstractions** over the hardware resources, and standard **services** that application programmers can use in their programs. In fact, the major focus of this course is to help you learn these abstractions and services, and how to use them in your programs. Due to this focus on learning, explaining, and programming using these abstractions and services, courses with similar content use titles like *"System Programming,"* *"Principles of Computer Systems,"* or *"Introduction to Computer Systems"* at some universities.

---

## Goals of an OS

A major goal of an OS is to provide a convenient software interface to hardware resources. The OS:

- Takes **physical resources** such as the processor, memory, and disk, and provides **software abstractions** over these resources.
- Works to **maximize utilization** of the hardware resources and to manage **contention** of resources among different users.
- Provides **standardized software libraries** to allow programs to interact with the provided abstractions.
- Provides **security** and supports **software development**.

---

## OS Services

Standard services provided by an OS include:

### 1. Process Management
- An OS is responsible for starting new programs and ending programs.
- The OS provides this functionality via the **process abstraction**, which we can think of as a running program.

### 2. File and Input/Output Management
- An OS provides **file** and **directory** abstractions, where a file organizes bits into meaningful structures and directories organize files into meaningful structures.
- An OS also provides interfaces for reading and writing files, as well as interfaces to communicate with external devices.

### 3. Inter-Process Communication (IPC)
- Processes running on the same machine as well as those running on different machines frequently need to communicate with each other. This is called **Inter-process Communication** or **IPC**.
- The OS provides support for IPC via various abstractions including **signals**, **pipes**, and **network sockets**.

### 4. Process Coordination
- Different processes running on an OS may need to access shared resources, e.g., files, devices, etc.
- Access to these resources may need to be coordinated, and the OS provides support for managing **concurrent access** to shared resources.

---

## OS Kernel

In addition to providing the services listed above, an OS typically provides additional services for application developers and users. These can include software to interact with the user, such as **shells** and **graphical user interfaces**, as well as libraries for application developers.

A distinction is generally made between the **OS Kernel** and the additional software for user interaction and application developers that is bundled with an OS but is not part of the kernel:

- The **kernel** is responsible for providing the standard services listed above and is the program that is **always running** on the computer.
- The kernel is allocated its own memory space called **kernel memory**.
- To keep an OS running even when applications fail or act maliciously, kernel memory must be **protected from direct access** by applications.
- Access to kernel services and memory is controlled by providing **system calls** — an application programming interface (API) provided by an OS to applications for interacting with the kernel.

---

## Interacting with the OS

At a higher level, we can consider two different types of interactions with an OS: interaction by **users** and interaction by **programs**.

### Interaction by Users

There are two primary modes by which users interact with the OS:

- **Command Line Shells** — These require the user to type commands to access OS services. Early OSs only supported command line shells.
- **Graphical User Interfaces (GUIs)** — Learning commands to type at a command line shell imposed a high learning curve on the end user. This prompted the development of GUIs where users can interact with the OS using graphical elements.

### Interaction by Programs

The OS exposes its services via **system calls**, which is the programmatic way a program requests services from an OS. When writing an application program, you may or may not have direct access to make the system calls. However, most programming languages provide APIs that make system calls behind the scenes. We will study system calls and APIs related to system calls in great detail in the course.

---

## Exercises

Think about all the electronic devices you use on a daily basis:

1. What OS is running on each of these devices?
2. What are the different interfaces you use to interact with each of these OSs?
3. Do you think the goals of these OSs may differ? If so, in what ways?

---

## Additional Resources

- [Timeline of Operating Systems](https://en.wikipedia.org/wiki/Timeline_of_operating_systems)
- [Usage Share of Operating Systems](https://en.wikipedia.org/wiki/Usage_share_of_operating_systems)
- [Graphical User Interfaces](https://en.wikipedia.org/wiki/Graphical_user_interface)
