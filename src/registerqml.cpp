#include "mousemover.h"
#include "mysimulator.h"
#include "highlighter.h"
#include "LammpsWrappers/simulatorcontrols/simulatorcontrol.h"
#include "LammpsWrappers/simulatorcontrols/cpcompute.h"
#include "LammpsWrappers/simulatorcontrols/cpfix.h"
#include "LammpsWrappers/simulatorcontrols/cpfixindent.h"
#include "LammpsWrappers/simulatorcontrols/cpvariable.h"
#include "LammpsWrappers/simulatorcontrols/nvt.h"
#include "LammpsWrappers/simulatorcontrols/rdf.h"
#include "performance.h"
#include "datasource.h"
#include "LammpsWrappers/atoms.h"
#include "LammpsWrappers/system.h"
#include "LammpsWrappers/groups.h"
#include "LammpsWrappers/fixes.h"
#include "LammpsWrappers/regions.h"
#include "LammpsWrappers/modifiers/modifiers.h"
#include "LammpsWrappers/computes.h"
#include "LammpsWrappers/variables.h"
#include "LammpsWrappers/units.h"
#include "dataproviders/data1d.h"
#include "dataproviders/data2d.h"
#include "dataproviders/dataprovider.h"
#include "linenumbers.h"
#include "usagestatistics.h"
#include "windowgl2.h"
#include "codeeditorbackend.h"
#include "states.h"
#include "parsefileuploader.h"
#include "standardpaths.h"
#include "keysequence.h"

void registerQML() {
    qmlRegisterType<AtomifySimulator>("Atomify", 1, 0, "AtomifySimulator");
    qmlRegisterType<CPCompute>("Atomify", 1, 0, "Compute");
    qmlRegisterUncreatableType<SimulatorControl>("Atomify", 1, 0, "SimulatorControl",
                                              "Cannot create abstract type SimulatorControl. This must be subclassed.");
    qmlRegisterType<CPFix>("Atomify", 1, 0, "Fix");
    qmlRegisterType<CPFixIndent>("Atomify", 1, 0, "FixIndent");
    qmlRegisterType<CPVariable>("Atomify", 1, 0, "Variable");
    qmlRegisterType<NVT>("Atomify", 1, 0, "NVT");
    // qmlRegisterType<RDF>("Atomify", 1, 0, "RDF");
    qmlRegisterType<DataSource>("Atomify", 1, 0, "DataSource");
    qmlRegisterType<Atoms>("Atomify", 1, 0, "Atoms");
    qmlRegisterType<System>("Atomify", 1, 0, "System");
    qmlRegisterType<Units>("Atomify", 1, 0, "Units");
    qmlRegisterType<Groups>("Atomify", 1, 0, "Groups");
    qmlRegisterType<Computes>("Atomify", 1, 0, "Computes");
    qmlRegisterType<Variables>("Atomify", 1, 0, "Variables");
    qmlRegisterType<Fixes>("Atomify", 1, 0, "Fixes");
    qmlRegisterType<Regions>("Atomify", 1, 0, "Regions");
    qmlRegisterType<DataProvider>("Atomify", 1, 0, "DataProvider");
    qmlRegisterType<Data1D>("Atomify", 1, 0, "Data1D");
    qmlRegisterType<Data2D>("Atomify", 1, 0, "Data2D");

    qmlRegisterType<ColorAllGroupsModifier>("Atomify", 1, 0, "ColorAllGroupsModifier");
    qmlRegisterType<ColorAllRegionsModifier>("Atomify", 1, 0, "ColorAllRegionsModifier");
    qmlRegisterType<ColorModifier>("Atomify", 1, 0, "ColorModifier");
    qmlRegisterType<PeriodicImages>("Atomify", 1, 0, "PeriodicImages");
    qmlRegisterType<GroupModifier>("Atomify", 1, 0, "GroupModifier");
    qmlRegisterType<RegionModifier>("Atomify", 1, 0, "RegionModifier");
    qmlRegisterType<PropertyModifier>("Atomify", 1, 0, "PropertyModifier");
    qmlRegisterType<Slice>("Atomify", 1, 0, "SliceModifier");

    qmlRegisterType<LineNumbers>("Atomify", 1, 0, "LineNumbers");
    qmlRegisterType<CodeEditorBackend>("Atomify", 1, 0, "CodeEditorBackend");
    qmlRegisterType<HighlighterWrapper>("Atomify", 1, 0, "Highlighter");
    qmlRegisterType<WindowGL2>("Atomify", 1, 0, "WindowGL2");
    qmlRegisterType<MouseMover>("Atomify", 1, 0, "MouseMover");
    qmlRegisterType<States>("Atomify", 1, 0, "States");
    qmlRegisterType<Performance>("Atomify", 1, 0, "Performance");
    qmlRegisterType<UsageStatistics>("Atomify", 1, 0, "UsageStatistics");

    qmlRegisterType<ParseFileUploader>("Atomify", 1, 0, "ParseFileUploader");

    qmlRegisterSingletonType<StandardPaths>("StandardPaths", 1, 0, "StandardPaths", &StandardPaths::qmlInstance);

    qmlRegisterSingletonType<KeySequence>("KeySequence", 1, 0, "KeySequence", &KeySequence::qmlInstance);
}
