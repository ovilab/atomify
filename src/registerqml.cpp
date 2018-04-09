#include "helpers/mousemover.h"
#include "helpers/windowgl2.h"
#include "helpers/standardpaths.h"
#include "helpers/keysequence.h"
#include "helpers/rendercapturehelper.h"
#include "highlighter.h"
#include "datasource.h"
#include "LammpsWrappers/modifiers/modifiers.h"
#include "dataproviders/data1d.h"
#include "dataproviders/data2d.h"
#include "dataproviders/dataprovider.h"
#include "linenumbers.h"
#include "usagestatistics.h"
#include "codeeditorbackend.h"
#include "states.h"
#include "parsefileuploader.h"
#include "helpers/clipboard.h"
#include "lammpscontroller.h"

using namespace atomify;

void registerQML() {
    qmlRegisterType<DataSource>("Atomify", 1, 0, "DataSource");
    qmlRegisterType<DataProvider>("Atomify", 1, 0, "DataProvider");
    qmlRegisterType<Data1D>("Atomify", 1, 0, "Data1D");
    qmlRegisterType<Data2D>("Atomify", 1, 0, "Data2D");

    qmlRegisterType<LineNumbers>("Atomify", 1, 0, "LineNumbers");
    qmlRegisterType<CodeEditorBackend>("Atomify", 1, 0, "CodeEditorBackend");
    qmlRegisterType<HighlighterWrapper>("Atomify", 1, 0, "Highlighter");
    qmlRegisterType<WindowGL2>("Atomify", 1, 0, "WindowGL2");
    qmlRegisterType<MouseMover>("Atomify", 1, 0, "MouseMover");
    qmlRegisterType<States>("Atomify", 1, 0, "States");
    qmlRegisterType<UsageStatistics>("Atomify", 1, 0, "UsageStatistics");
    qmlRegisterType<Clipboard>("Atomify", 1, 0, "Clipboard");

    qmlRegisterType<ParseFileUploader>("Atomify", 1, 0, "ParseFileUploader");

    qmlRegisterSingletonType<StandardPaths>("StandardPaths", 1, 0, "StandardPaths", &StandardPaths::qmlInstance);

    qmlRegisterSingletonType<KeySequence>("KeySequence", 1, 0, "KeySequence", &KeySequence::qmlInstance);

    qmlRegisterSingletonType<RenderCaptureHelper>("Atomify", 1, 0, "RenderCaptureHelper", &RenderCaptureHelper::qmlInstance);

    qmlRegisterType<LAMMPSController>("Atomify", 1, 0, "LAMMPSController");
}
