# Package:  core
# Date:     2nd April 2006
# Author:   James Mills, prologic at shortcircuit dot net dot au

"""Core

This package contains the essential core parts of the circuits framework.
"""

from handlers import handler

from events import Event
from manager import Manager
from components import BaseComponent, Component

from values import Value
from futures import future, Future

try:
    from rexviewer import logInfo as log
except ImportError:
    from logging import info as log

from timers import Timer
try:
    from bridge import Bridge
except:
    import traceback as tb
    log(tb.format_exc())
    print "Failed to import circuits.core.bridge. This probably means we're running in debug mode in Windows, and the debug version of the Python socket library is not available. Continuing without."
    Bridge = None
from debugger import Debugger
from workers import Thread, Process

__all__ = ("handler",
        "Event", "Manager", "BaseComponent", "Component",
        "Value", "future", "Future",
        "Timer", "Bridge", "Debugger",
        "Thread", "Process",)
