//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum ViewManager::ViewOperationType
{
	VIEWOPERATIONTYPE_OPEN,
	VIEWOPERATIONTYPE_CLOSE,
	VIEWOPERATIONTYPE_DELETE,
	VIEWOPERATIONTYPE_ACTIVATE,
	VIEWOPERATIONTYPE_SHOW,
	VIEWOPERATIONTYPE_HIDE,
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
	boost::condition viewClosed;
	boost::condition viewOpened;
	boost::condition viewInfoSafeToDelete;
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
struct ViewManager::WorkspaceViewEntryDetails
{
	WorkspaceViewEntryDetails(const std::wstring& aviewGroupName, const std::wstring& aviewName, IViewPresenter::ViewTarget aviewTarget, unsigned int amoduleID = 0, const std::wstring& ainstanceName = L"", bool aglobalExtension = false)
	:viewGroupName(aviewGroupName), viewName(aviewName), viewTarget(aviewTarget), instanceName(ainstanceName), globalExtension(aglobalExtension), moduleID(amoduleID)
	{}

	std::wstring viewGroupName;
	std::wstring viewName;
	IViewPresenter::ViewTarget viewTarget;
	unsigned int moduleID;
	std::wstring instanceName;
	bool globalExtension;
};

//----------------------------------------------------------------------------------------
struct ViewManager::PlaceholderWindowInfo
{
	PlaceholderWindowInfo()
	:placeholderContentWindow(NULL), parentWindowFrame(NULL), makeContentVisible(false), selectedContentWindow(false), viewType(IView::VIEWTYPE_DOCKABLE)
	{}

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
	InvokeUIParams(const boost::function<void()>& acallback, boost::condition& acallbackComplete)
	:callback(acallback), callbackComplete(acallbackComplete)
	{}

	const boost::function<void()>& callback;
	boost::condition& callbackComplete;
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
