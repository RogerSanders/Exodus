//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class ViewManager::ViewOperationType
{
	Open,
	Close,
	Delete,
	Activate,
	Show,
	Hide,
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct ViewManager::ViewInfo
{
	ViewInfo(IViewPresenter& aviewPresenter)
	:viewPresenter(aviewPresenter), waitCount(0), viewCurrentlyOpen(false), viewOpenFailed(false), ignoreForLayoutPurposes(false), node(0)
	{}

	bool viewCurrentlyOpen;
	bool viewOpenFailed;
	bool ignoreForLayoutPurposes;
	int waitCount;
	IViewPresenter& viewPresenter;
	ViewStateChangeNotifier* notifier;
	std::condition_variable_any viewClosed;
	std::condition_variable_any viewOpened;
	std::condition_variable_any viewInfoSafeToDelete;
	IHierarchicalStorageNode* node;
};

//----------------------------------------------------------------------------------------
struct ViewManager::ViewOperation
{
	ViewOperation(ViewOperationType atype, IViewPresenter& aviewPresenter, IHierarchicalStorageNode* aviewState = 0)
	:type(atype), viewPresenter(aviewPresenter), viewState(aviewState)
	{}

	ViewOperationType type;
	IViewPresenter& viewPresenter;
	IHierarchicalStorageNode* viewState;
};

//----------------------------------------------------------------------------------------
struct ViewManager::PlaceholderWindowInfo
{
	PlaceholderWindowInfo()
	:parentIsDashboardWindow(false), placeholderContentWindow(NULL), parentWindowFrame(NULL), makeContentVisible(false), selectedContentWindow(false), viewType(IView::ViewType::Dockable)
	{}

	bool parentIsDashboardWindow;
	HWND placeholderContentWindow;
	HWND parentWindowFrame;
	bool makeContentVisible;
	bool selectedContentWindow;
	IView::ViewType viewType;
};

//----------------------------------------------------------------------------------------
struct ViewManager::OpenWindowInfo
{
	OpenWindowInfo(HWND ahwnd, IView& aview, IViewPresenter& aviewPresenter, std::wstring awindowTitle = L"")
	:hwnd(ahwnd), view(aview), viewPresenter(aviewPresenter), windowTitle(awindowTitle)
	{}

	HWND hwnd;
	IView& view;
	IViewPresenter& viewPresenter;
	std::wstring windowTitle;
};

//----------------------------------------------------------------------------------------
struct ViewManager::InvokeUIParams
{
	InvokeUIParams(const std::function<void()>& acallback, std::condition_variable& acallbackComplete)
	:callback(acallback), callbackComplete(acallbackComplete)
	{}

	const std::function<void()>& callback;
	std::condition_variable& callbackComplete;
};

//----------------------------------------------------------------------------------------
struct ViewManager::Region2D
{
	Region2D()
	{}
	Region2D(long aposx, long aposy, long asizex, long asizey)
	:posx(aposx), posy(aposy), sizex(asizex), sizey(asizey)
	{}

	long posx;
	long posy;
	long sizex;
	long sizey;
};

//----------------------------------------------------------------------------------------
struct ViewManager::DialogWindowFrameState
{
	DialogWindowFrameState()
	:viewManager(0)
	{}

	std::list<HWND> disabledWindowList;
	ViewManager* viewManager;
};
