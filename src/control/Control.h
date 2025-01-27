/*
 * Xournal++
 *
 * The main Control
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include "Actions.h"
#include "ClipboardHandler.h"
#include "RecentManager.h"
#include "ScrollHandler.h"
#include "ToolHandler.h"
#include "AudioController.h"

#include "gui/MainWindow.h"
#include "gui/SearchBar.h"
#include "gui/sidebar/Sidebar.h"
#include "jobs/ProgressListener.h"
#include "jobs/XournalScheduler.h"
#include "model/Document.h"
#include "settings/MetadataManager.h"
#include "settings/Settings.h"
#include "undo/UndoRedoHandler.h"
#include "zoom/ZoomControl.h"

#include <XournalType.h>
#include <PathUtil.h>

#include "../gui/dialog/LatexDialog.h"

class AudioController;
class FullscreenHandler;
class Sidebar;
class XojPageView;
class SaveHandler;
class GladeSearchpath;
class MetadataManager;
class XournalppCursor;
class ToolbarDragDropHandler;
class MetadataEntry;
class MetadataCallbackData;
class PageBackgroundChangeController;
class PageTypeHandler;
class PageTypeMenu;
class BaseExportJob;
class LayerController;
class PluginController;

class Control :
	public ActionHandler,
	public ToolListener,
	public DocumentHandler,
	public RecentManagerListener,
	public UndoRedoListener,
	public ClipboardListener,
	public ProgressListener
{
public:
	Control(GladeSearchpath* gladeSearchPath);
	virtual ~Control();

	void initWindow(MainWindow* win);
public:
	// Menu File
	bool newFile(string pageTemplate = "");
	bool openFile(Path filename = "", int scrollToPage = -1, bool forceOpen = false);
	bool annotatePdf(Path filename, bool attachPdf, bool attachToDocument);
	void print();
	void exportAsPdf();
	void exportAs();
	void exportBase(BaseExportJob* job);
	void quit(bool allowCancel = true);

	/**
	 * Save the current document.
	 *
	 * @param synchron Whether the save should be run synchronously or asynchronously.
	 */
	bool save(bool synchron = false);
	bool saveAs();

	/**
	 * Marks the current document as saved if it is currently marked as unsaved.
	 */
	void resetSavedStatus();

	/**
	 * Close the current document, prompting to save unsaved changes.
	 *
	 * @param allowDestroy Whether clicking "Discard" should destroy the current document.
	 * @param allowCancel Whether the user should be able to cancel closing the document.
	 * @return true if the user closed the document, otherwise false.
	 */
	bool close(bool allowDestroy = false, bool allowCancel = true);

	/**
	 * Calls close, always forcing the document to be destroyed.
	 * @return The value returned by close
	 */
	bool closeAndDestroy(bool allowCancel = false);

	// Asks user to replace an existing file when saving / exporting, since we add the extension
	// after the OK, we need to check manually
	bool checkExistingFile(Path& folder, Path& filename);

	void resetShapeRecognizer();

	// Menu edit
	void showSettings();

	// The core handler for inserting latex
	void runLatex();

	// Menu Help
	void showAbout();

	virtual void actionPerformed(ActionType type, ActionGroup group, GdkEvent* event, GtkMenuItem* menuitem,
								 GtkToolButton* toolbutton, bool enabled);

	/**
	 * Select the color for the tool
	 *
	 * @param userSelection
	 * 			true if the user selected the color
	 * 			false if the color is selected by a tool change
	 * 			and therefore should not be applied to a selection
	 */
	virtual void toolColorChanged(bool userSelection);
	virtual void setCustomColorSelected();
	virtual void toolChanged();
	virtual void toolSizeChanged();
	virtual void toolFillChanged();
	virtual void toolLineStyleChanged();

	void selectTool(ToolType type);
	void selectDefaultTool();

	void updatePageNumbers(size_t page, size_t pdfPage);

	virtual void fileOpened(const char* uri);

	/**
	 * Save current state (selected tool etc.)
	 */
	void saveSettings();

	void updateWindowTitle();
	void setViewPairedPages(bool enabled);
	void setViewPresentationMode(bool enabled);
	void setPairsOffset(int numOffset);
	void setViewColumns(int numColumns);
	void setViewRows(int numRows);
	void setViewLayoutVert(bool vert);
	void setViewLayoutR2L(bool r2l);
	void setViewLayoutB2T(bool b2t);
	
	void manageToolbars();
	void customizeToolbars();
	void setFullscreen(bool enabled);

	void gotoPage();

	void setShapeTool(ActionType type, bool enabled);

	void paperTemplate();
	void paperFormat();
	void changePageBackgroundColor();
	void updateBackgroundSizeButton();

	void endDragDropToolbar();
	void startDragDropToolbar();
	bool isInDragAndDropToolbar();

	bool isFullscreen();

	bool searchTextOnPage(string text, int p, int* occures, double* top);

	/**
	 * Fire page selected, but first check if the page Number is valid
	 *
	 * @return the page ID or size_t_npos if the page is not found
	 */
	size_t firePageSelected(PageRef page);
	void firePageSelected(size_t page);

	void addDefaultPage(string pageTemplate);
	void insertNewPage(size_t position);
	void insertPage(const PageRef& page, size_t position);
	void deletePage();

	/**
	 * Disable / enable delete page button
	 */
	void updateDeletePageButton();

	// selection handling
	void clearSelection();

	void setCopyPasteEnabled(bool enabled);

	void enableAutosave(bool enable);

	void clearSelectionEndText();

	void setToolSize(ToolSize size);
	void setToolColor(int color);

	void setLineStyle(const string& style);

	void setFill(bool fill);

	TextEditor* getTextEditor();

	GladeSearchpath* getGladeSearchPath();

	void disableSidebarTmp(bool disabled);

	XournalScheduler* getScheduler();

	void block(string name);
	void unblock();

	void renameLastAutosaveFile();
	void setLastAutosaveFile(Path newAutosaveFile);
	void deleteLastAutosaveFile(Path newAutosaveFile);
	void setClipboardHandlerSelection(EditSelection* selection);

	MetadataManager* getMetadataManager();
	Settings* getSettings();
	ToolHandler* getToolHandler();
	ZoomControl* getZoomControl();
	Document* getDocument();
	UndoRedoHandler* getUndoRedoHandler();
	MainWindow* getWindow();
	GtkWindow* getGtkWindow();
	RecentManager* getRecentManager();
	ScrollHandler* getScrollHandler();
	PageRef getCurrentPage();
	size_t getCurrentPageNo();
	XournalppCursor* getCursor();
	Sidebar* getSidebar();
	SearchBar* getSearchBar();
	AudioController* getAudioController();
	PageTypeHandler* getPageTypes();
	PageTypeMenu* getNewPageType();
	PageBackgroundChangeController* getPageBackgroundChangeController();
	LayerController* getLayerController();


	bool copy();
	bool cut();
	bool paste();

	void help();

	void selectFillAlpha(bool pen);

public:
	// UndoRedoListener interface
	void undoRedoChanged();
	void undoRedoPageChanged(PageRef page);

public:
	// ProgressListener interface
	void setMaximumState(int max);
	void setCurrentState(int state);

public:
	// ClipboardListener interface
	virtual void clipboardCutCopyEnabled(bool enabled);
	virtual void clipboardPasteEnabled(bool enabled);
	virtual void clipboardPasteText(string text);
	virtual void clipboardPasteImage(GdkPixbuf* img);
	virtual void clipboardPasteXournal(ObjectInputStream& in);
	virtual void deleteSelection();

	void clipboardPaste(Element* e);

protected:
	/**
	 * This callback is used by used to be called later in the UI Thread
	 * On slower machine this feels more fluent, therefore this will not
	 * be removed
	 */
	void zoomCallback(ActionType type, bool enabled);

	void rotationSnappingToggle();
	void gridSnappingToggle();

	bool showSaveDialog();

	void fileLoaded(int scrollToPage = -1);

	void eraserSizeChanged();
	void penSizeChanged();
	void hilighterSizeChanged();

	static bool checkChangedDocument(Control* control);
	static bool autosaveCallback(Control* control);

	void fontChanged();
	/**
	 * Load metadata later, md will be deleted
	 */
	void loadMetadata(MetadataEntry md);

	static bool loadMetadataCallback(MetadataCallbackData* data);

	/**
	 * Check if this is an autosave file, return false in this case and display a user instruction
	 */
	bool shouldFileOpen(string filename);

	bool loadXoptTemplate(Path filename);
	bool loadPdf(const Path& filename, int scrollToPage);

private:
	XOJ_TYPE_ATTRIB;

	/**
	 * "Closes" the document, preparing the editor for a new document.
	 */
	void closeDocument();

	RecentManager* recent;
	UndoRedoHandler* undoRedo;
	ZoomControl* zoom;

	Settings* settings;
	MainWindow* win = NULL;

	Document* doc = NULL;

	Sidebar* sidebar = NULL;
	SearchBar* searchBar = NULL;

	ToolHandler* toolHandler;

	ActionType lastAction;
	ActionGroup lastGroup;
	bool lastEnabled;

	ScrollHandler* scrollHandler;

	AudioController* audioController;

	ToolbarDragDropHandler* dragDropHandler = NULL;

	/**
	 * The cursor handler
	 */
	XournalppCursor* cursor;

	/**
	 * Timeout id: the timeout watches the changes and actualizes the previews from time to time
	 */
	int changeTimout;

	/**
	 * The pages wihch has changed since the last update (for preview update)
	 */
	std::vector<XojPage*> changedPages;

	/**
	 * Our clipboard abstraction
	 */
	ClipboardHandler* clipboardHandler = NULL;

	/**
	 * The autosave handler ID
	 */
	int autosaveTimeout = 0;
	Path lastAutosaveFilename;

	XournalScheduler* scheduler;

	/**
	 * State / Blocking attributes
	 */
	GtkWidget* statusbar = NULL;
	GtkLabel* lbState = NULL;
	GtkProgressBar* pgState = NULL;
	int maxState = 0;
	bool isBlocking;

	GladeSearchpath* gladeSearchPath;

	MetadataManager* metadata;

	PageTypeHandler* pageTypes;
	PageTypeMenu* newPageType;

	PageBackgroundChangeController* pageBackgroundChangeController;

	LayerController* layerController;

	/**
	 * Manage all Xournal++ plugins
	 */
	PluginController* pluginController;

	/**
	 * Fullscreen handler
	 */
	FullscreenHandler* fullscreenHandler;
};
