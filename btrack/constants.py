import os
import enum

BTRACK_PATH = os.path.dirname(os.path.abspath(__file__))

def get_version():
    with open(os.path.join(BTRACK_PATH, "VERSION.txt") ,"r") as ver:
        version = ver.readline()
    return version.rstrip()

# __version__ = get_version()
DEFAULT_LOW_PROBABILITY = -1e5
MAX_LOST = 5
PROB_NOT_ASSIGN = 0.1
DEBUG = True
EXPORT_FORMATS = frozenset(['.json','.mat','.hdf5'])
NEW_COLORS = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b',
                '#e377c2', '#7f7f7f', '#bcbd22', '#17becf']
VOLUME = ((0,1024), (0,1024), (-100,100))
HDF_CHUNK_CACHE = 100*1024*1024
USER_MODEL_DIR = ""


@enum.unique
class Errors(enum.Enum):
    SUCCESS = 900
    EMPTY_QUEUE = 901
    NO_TRACKS = 902
    NO_USEABLE_FRAMES = 903
    TRACK_EMPTY = 904
    INCORRECT_MOTION_MODEL = 905
    MAX_LOST_OUT_OF_RANGE = 906
    ACCURACY_OUT_OF_RANGE = 907
    PROB_NOT_ASSIGN_OUT_OF_RANGE = 908
    NOT_DEFINED = 909
    NO_ERROR = 910

@enum.unique
class Fates(enum.Enum):
    FALSE_POSITIVE = 0
    INITIALIZE = 1
    TERMINATE = 2
    LINK = 3
    DIVIDE = 4
    APOPTOSIS = 5
    MERGE = 6
    EXTRUDE = 7
    INITIALIZE_BORDER = 10
    INITIALIZE_FRONT = 11
    TERMINATE_BORDER = 20
    TERMINATE_BACK = 21
    DEAD = 666
    UNDEFINED = 999

@enum.unique
class States(enum.Enum):
    INTERPHASE = 0
    PROMETAPHASE = 1
    METAPHASE = 2
    ANAPHASE = 3
    APOPTOSIS = 4
    NULL = 5
    DUMMY = 99
