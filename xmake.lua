target("simpleEDA")
    add_rules("qt.widgetapp")

    set_languages("cxx17")
    add_toolchains("clang")
    set_warnings("all")
    set_optimize("fast")
    set_targetdir(".")

    add_includedirs("include/")
    add_linkdirs("lib/")
    add_rpathdirs("lib/")
    add_links("armadillo")
    add_links("qcustomplot")


    add_files("src/mainwindow/mainwindow.h")
    add_headerfiles("src/**.h | mainwindow.h")
    add_files("src/**.cpp")
    add_files("src/**.qrc")

