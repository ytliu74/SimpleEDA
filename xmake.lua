add_rules("mode.release")

target("simpleEDA")
    add_rules("qt.widgetapp")

    set_languages("cxx17")
    add_toolchains("clang")

    add_files("src/mainwindow/mainwindow.h")
    add_headerfiles("src/**.h | mainwindow.h")
    add_files("src/**.cpp")
    add_files("src/**.qrc")

