# iri_edp

IRI_EDP is a sample application written in C that integrates with the International Reference Ionosphere model (http://irimodel.org) to generate an electron density profile for a given location and date and time. This application includes a Makefile that compiles the IRI 2016 model as a set of Fortran static libraries and links them with a C program that collects required inputs to the model and calls the Fortran code to collect the required data points. It then calls GNUPlot to generate a PDF version of the electron density profile chart.

# Prerequisites
This tool can be built either locally or using Docker. If building locally, you will need a suitable C compiler (I have tested both Apple clang and GNU C), a Fortran compiler (I have only tested GNU Fortran), the wget utility (for fetching files from https://irimodel.org), and an installation of GNUplot. Building and running with Docker should not require any specific host-installed utilities other than Docker and internet access to public registries and Ubuntu repositories.

# Building
Both ways of building the tools can be driven through the provided Makefile. To build locally, you can simply execute `make` in the repository folder. This will build all needed object files, link the final executable named "iri_edp", and place it and all needed common files, indices, etc. into the "build" folder. To build using Docker, execute the command `make docker` in the repository folder. This will build a Docker image locally that contains the executable and all dependencies needed to run it. This docker image will be configured to run iri_edp as its default entrypoint.

# Running
Running the tool interactively can be accomplished by changing working directory to the "build" directory created by make and running the 'iri_edp' command. This should prompt for the date/time information and geographic coordinates interactively on stdin. You can pipe input from a text file to stdin using a command like `iri_edp < test.txt` to avoid needing to enter it interactively. The two sample files input1.txt and input2.txt are set up to be used via a docker-test Makefile target; if you want to use them with a local build you will need to change the last line specifying the PDF output file to a location accessible locally.
To run the tool using Docker, you can manually run this command: `docker run -it iri` which will start a container running the iri image built by the Makefile and attach to it with an interactive TTY session. To pipe content from a text file to stdin of the container process, omit the 't' in the above command and pipe the text after the docker command. Note that the output of the command, a PDF file, is written to a location inside the container's filesystem: you must bind-mount a host folder when running the Docker container in order for the file to be written to a host folder.
To demonstrate this, I created an additional Makefile target docker-test which runs the Docker command twice, once for each of the supplied sample input text files, and bind-mounts the build folder on the container so that the output pdf charts are accessible.

# Lessons / Insights
I learned many lessons and insights through this process. The first lesson is that executing Fortran code from C is somewhat challenging if you have not worked with it in the past. My first attempt was to build the iritest Fortran executable and try to drive that from the C code using a pipe. This turned out to be challenging for me in the sense that there was a lot of information in the output text file that made charting it difficult without additional manipulation. So I ended up going down the route of figuring out how to call the Fortran entrypoint directly from linked code which was more accurate to the exercise requirements actually. This had its own challenges, mainly I had to learn enough of the Fortran syntax to write a wrapper subroutine that would adopt the C bindings needed to call the entrypoint from C. Once I did this, I could execute the Fortan successfully, although I still had some minor issues. For instance, I had to dig through some of the Fortran code to discover that I needed to call a separate set of Fortran subroutines to load the indices required by the model. Thankfully, the wrapper subroutine I created was an ideal place to do this using Fortran directly without needing to wrap additional subroutines to call them from C.
One additional challenge I encountered was handling simple string input in the C code. Discovering that using the tried-and-true scanf is no longer considered secure especially with arbitrary string input, I elected to use a different method that used readline to read input and allocate a buffer on the heap guaranteed to hold the input. This worked great, except this method leaves a newline at the end of the input string. I didn't discover this until much later when I used the input date/time string as part of the chart title when driving GNUplot, since the date/time parsing ignored the additional whitespace. I was able to work through this easy enough, but figuring out why my GNUplot commands were blowing up took some time to figure out.
Finally, I was not able to figure out how to generate an output of plasma frequency as shown in the sample chart vs. the raw electron density that I ended up using. Perhaps I missed some option in the Fortran inputs that would have yielded this value, but I could not find it documented in the code. Hopefully using the electron density is acceptable since that was the description of the problem.

One last challenge that I encountered: in the Docker build of the tool, I attempted to build a lightweight image to execute this tool, but that ended up being impossible in the short term as the required gnuplot utility pulled in a large number of dependencies for its Qt terminal support. Ideally, I would build a custom package of gnuplot that excludes any terminal support except for PDF which would result in a much smaller image, but I decided to leave that as a future challenge if time allowed.