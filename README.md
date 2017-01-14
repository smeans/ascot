# ascot
This project is the product of me spending two hours a week mentoring Alex, a bright middle schooler who is an aspiring engineer. We were taking a break from a hardware project we'd been working on and decided to make a simple web-based command prompt application in C.

It's pretty simple and has very few dependencies. To build it, just clone or download the sources and run the `build` script in the `src` folder. After that, run `ascot` and you'll have a very bare-bones HTTP server that presents a simple command prompt in the browser. It is visible on port 5000 by default, so just go to [http://localhost:5000](http://localhost:5000) to see your prompt.

## ====== SECURITY WARNING ======
## By default, this app spawns a `bash` shell under whatever user was used to start the server process. There is no client-side authentication, so this is obviously *very* insecure. Use at your own risk!
## ==============================
