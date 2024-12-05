#pragma once
#include <cassert>
#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include <string>

/**
 * <pre>
 * INI 文件格式的结构如下：
 * [文件根]
 *     |--[空行]
 *     |--[注释]
 *     +--[分节]
 *         |--[空行]
 *         |--[注释]
 *         |--[键值]
 *         |--[键值]
 *         |--[空行]
 *         |--[空行]
 *         |--[...]
 *     |--[空行]
 *     |--[空行]
 *     |--[空行]
 *     +--[分节]
 *           |--[空行]
 *           |--[注释]
 *           |--[键值]
 *           |--[空行]
 *           |--[键值]
 *           |--[键值]
 *           |--[键值]
 *           |--[空行]
 *           |--[...]
 *     |--[空行]
 *     |--[...]
 *
 * 文件根：INI 文件的虚拟名称，不存在于文件内容中。
 * 空行：空白行，即便有空白字符占据，也算空白行。
 * 注释：以 “;” 或者 “#” 开头后的内容，都算是注释内容。
 * 分节：格式为 “[section]” 。
 * 键值：格式为 “key=value” 。
 * </pre>
 */

/**
 * @enum  xini_node_type_t
 * @brief INI 文件中的节点信息类型。
 */
using xini_ntype_t = enum xini_ntype_t {
    kXiniNtypeUndefine = 0xFFFFFFFF,  ///< 未定义
    kXiniNtypeFileroot = 0x00000000,  ///< 文件根
    kXiniNtypeNilline = 0x00000100,   ///< 空行
    kXiniNtypeComment = 0x00000200,   ///< 注释
    kXiniNtypeSection = 0x00000300,   ///< 分节
    kXiniNtypeKeyvalue = 0x00000400,  ///< 键值
};

/** 前置声明相关的 INI 节点类 */
class XiniKeyvalueT;
class XiniSectionT;
class XiniCommentT;
class XiniNillineT;
class XiniFileT;

/** 字符串修剪操作的默认字符集（空白字符集，以 isspace() 判断的字符为标准） */
static const char kXcharsTrim[] = " \t\n\r\f\v";

/**
 * @class xini_node_t
 * @brief INI 节点描述基类。
 */
class XiniNodeT {
    friend class XiniFileT;
    friend class XiniSectionT;
    friend class XiniKeyvalueT;

    // common invoking
   protected:
    /**********************************************************/
    /**
     * @brief 判断是否为单行字符串。
     */
    static bool IsSline(const std::string &xstr) {
        return (xstr.find_first_of("\r\n") == std::string::npos);
    }

    /**********************************************************/
    /**
     * @brief 判定字符串是否被修剪过。
     */
    static bool IsXtrim(const std::string &xstr, const char *xchars = kXcharsTrim) {
        std::string::size_type st_pos = xstr.find_first_of(xchars);
        return ((std::string::npos == st_pos) || ((st_pos > 0) && (st_pos < (xstr.size() - 1))));
    }

    /**********************************************************/
    /**
     * @brief 修剪字符串前后端的字符集。
     */
    static std::string TrimXstr(const std::string &xstr, const char *xchars = kXcharsTrim) {
        std::string::size_type st_pos = xstr.find_first_not_of(xchars);
        if (std::string::npos != st_pos) {
            return xstr.substr(st_pos, xstr.find_last_not_of(xchars) - st_pos + 1);
        }

        return std::string("");
    }

    /**********************************************************/
    /**
     * @brief 修剪字符串前端的字符集。
     */
    static std::string TrimLstr(const std::string &xstr, const char *xchars = kXcharsTrim) {
        std::string::size_type st_pos = xstr.find_first_not_of(xchars);
        if (std::string::npos != st_pos) {
            return xstr.substr(st_pos);
        }

        return std::string("");
    }

    /**********************************************************/
    /**
     * @brief 修剪字符串后端的字符集。
     */
    static std::string TrimRstr(const std::string &xstr, const char *xchars = kXcharsTrim) {
        return xstr.substr(0, xstr.find_last_not_of(xchars));
    }

    /**********************************************************/
    /**
     * @brief 字符串忽略大小写的比对操作。
     *
     * @param [in ] xszt_lcmp : 比较操作的左值字符串。
     * @param [in ] xszt_rcmp : 比较操作的右值字符串。
     *
     * @return int
     *         - xszt_lcmp <  xszt_rcmp，返回 <= -1；
     *         - xszt_lcmp == xszt_rcmp，返回 ==  0；
     *         - xszt_lcmp >  xszt_rcmp，返回 >=  1；
     */
    static int XstrIcmp(const char *xszt_lcmp, const char *xszt_rcmp) {
        int xit_lvalue = 0;
        int xit_rvalue = 0;

        if (xszt_lcmp == xszt_rcmp) return 0;
        if (nullptr == xszt_lcmp) return -1;
        if (nullptr == xszt_rcmp) return 1;

        do {
            if (((xit_lvalue = (*(xszt_lcmp++))) >= 'A') && (xit_lvalue <= 'Z'))
                xit_lvalue -= ('A' - 'a');

            if (((xit_rvalue = (*(xszt_rcmp++))) >= 'A') && (xit_rvalue <= 'Z'))
                xit_rvalue -= ('A' - 'a');

        } while (xit_lvalue && (xit_lvalue == xit_rvalue));

        return (xit_lvalue - xit_rvalue);
    }

    /**
     * @struct xstr_icmp_t
     * @brief  as functor.
     */
    struct XstrIcmpT {
        using first_argument_type = std::string;
        using second_argument_type = std::string;
        using result_type = bool;

        bool operator()(const std::string &xstr_left, const std::string &xstr_right) const {
            return (XstrIcmp(xstr_left.c_str(), xstr_right.c_str()) < 0);
        }
    };

    // constructor/destructor

    XiniNodeT(int xini_ntype, XiniNodeT *xowner_ptr)
        : m_xini_ntype(xini_ntype), m_xowner_ptr(xowner_ptr) {}

    virtual ~XiniNodeT() = default;

    // extensible interfaces
   public:
    /**********************************************************/
    /**
     * @brief 将 节点信息 导向 输出流，派生的子类中必须实现具体操作。
     */
    virtual const XiniNodeT &operator>>(std::ostream &ostr) const = 0;

    /**********************************************************/
    /**
     * @brief 脏标识。
     */
    virtual bool IsDirty() const {
        if (nullptr != m_xowner_ptr) {
            return m_xowner_ptr->IsDirty();
        }
        return false;
    }

    /**********************************************************/
    /**
     * @brief 设置脏标识。
     */
    virtual void SetDirty(bool x_dirty) {
        if (nullptr != m_xowner_ptr) {
            m_xowner_ptr->SetDirty(x_dirty);
        }
    }

   protected:
    /**********************************************************/
    /**
     * @brief 重命名附属的子节点（分节节点、键值节点）的索引名。
     */
    virtual bool RenameNsub(XiniNodeT * /*xnsub_ptr*/, const std::string & /*xstr_name*/) {
        return false;
    }

    // public interfaces
   public:
    /**********************************************************/
    /**
     * @brief 节点类型。
     */
    int Ntype() const { return m_xini_ntype; }

    /**********************************************************/
    /**
     * @brief 获取节点的持有者。
     */
    XiniNodeT *GetOwner() const { return m_xowner_ptr; }

    // data members
   protected:
    int m_xini_ntype;         ///< 节点类型
    XiniNodeT *m_xowner_ptr;  ///< 节点持有者
};

/**********************************************************/
/**
 * @brief 定义 xini_node_t 的流输出操作符函数。
 */
inline std::ostream &operator<<(std::ostream &ostr, const XiniNodeT &xini_node) {
    xini_node >> ostr;
    return ostr;
}

////////////////////////////////////////////////////////////////////////////////
// xini_nilline_t

/**
 * @class xini_nilline_t
 * @brief INI 文件中的空行节点类。
 */
class XiniNillineT : public XiniNodeT {
    friend class XiniFileT;

    // common invoking
   protected:
    /**********************************************************/
    /**
     * @brief 尝试使用字符串直接创建并初始化 xini_nilline_t 对象。
     *
     * @param [in ] xstr_line  :
     *  用于创建 空行节点 的字符串行，
     *  其已经被 trim_xstr() 修剪前后端的空白字符。
     *
     * @param [in ] xowner_ptr :
     *  键值节点的拥有者（xini_section_t 类型）。
     *
     * @return xini_node_t * :
     *  操作成功，返回的 空行节点；若失败，则返回 nullptr 。
     */
    static XiniNodeT *try_create(const std::string &xstr_line, XiniNodeT *xowner_ptr) {
        assert(is_xtrim(xstr_line));
        assert(is_sline(xstr_line));

        if (!xstr_line.empty()) {
            return nullptr;
        }

        return (new XiniNillineT(xowner_ptr));
    }

    // construcor/destructor

    explicit XiniNillineT(XiniNodeT *xowner_ptr) : XiniNodeT(kXiniNtypeNilline, xowner_ptr) {}

    ~XiniNillineT() override = default;

    // overrides
   public:
    /**********************************************************/
    /**
     * @brief 将 节点信息 导向 输出流。
     */
    const XiniNodeT &operator>>(std::ostream &ostr) const override {
        ostr << std::endl;
        return *this;
    }
};

////////////////////////////////////////////////////////////////////////////////
// xini_comment_t

/**
 * @class xini_comment_t
 * @brief INI 文件中的 注释 节点类。
 */
class XiniCommentT : public XiniNodeT {
    friend class XiniFileT;

    // common invoking
   protected:
    /**********************************************************/
    /**
     * @brief 尝试使用字符串直接创建并初始化 xini_comment_t 对象。
     *
     * @param [in ] xstr_line  :
     *  用于创建 注释节点 的字符串行，
     *  其已经被 trim_xstr() 修剪前后端的空白字符。
     *
     * @param [in ] xowner_ptr :
     *  键值节点的拥有者（xini_section_t 类型）。
     *
     * @return xini_node_t * :
     *  操作成功，返回的 注释节点；若失败，则返回 nullptr 。
     */
    static XiniNodeT *try_create(const std::string &xstr_line, XiniNodeT *xowner_ptr) {
        assert(is_xtrim(xstr_line));
        assert(is_sline(xstr_line));

        if (xstr_line.empty() || ((';' != xstr_line.at(0)) && ('#' != xstr_line.at(0)))) {
            return nullptr;
        }

        auto *xnode_ptr = new XiniCommentT(xowner_ptr);
        xnode_ptr->m_xstr_text_ = xstr_line;
        return xnode_ptr;
    }

    // construcor/destructor

    explicit XiniCommentT(XiniNodeT *xowner_ptr) : XiniNodeT(kXiniNtypeComment, xowner_ptr) {}

    ~XiniCommentT() override = default;

    // overrides
   public:
    /**********************************************************/
    /**
     * @brief 将 节点信息 导向 输出流。
     */
    const XiniNodeT &operator>>(std::ostream &ostr) const override {
        ostr << m_xstr_text_ << std::endl;
        return *this;
    }

    /**********************************************************/
    /**
     * @brief 注释行字符串 内容。
     */
    const std::string &Text() const { return m_xstr_text_; }

   protected:
    std::string m_xstr_text_;  ///< 注释行字符串
};

////////////////////////////////////////////////////////////////////////////////
// xini_keyvalue_t

/**
 * @class xini_keyvalue_t
 * @brief INI 文件中的 分节 节点类。
 */
class XiniKeyvalueT : public XiniNodeT {
    friend class XiniFileT;
    friend class XiniSectionT;

    // common invoking
   protected:
    /**********************************************************/
    /**
     * @brief 检查键名字符串格式是否有效。
     *
     * @param [in ] xstr_name :
     *  待检查的 键名，其已经被 trim_xstr() 修剪过前后端的空白字符。
     */
    static bool CheckKname(const std::string &xstr_name) {
        assert(is_xtrim(xstr_name));

        if (xstr_name.empty()) {
            return false;
        }

        if (std::string::npos != xstr_name.find_first_of(";#=\r\n")) {
            return false;
        }

        if (('[' == xstr_name.at(0)) && (std::string::npos != xstr_name.find(']'))) {
            return false;
        }

        return true;
    }

    /**********************************************************/
    /**
     * @brief 尝试使用字符串直接创建并初始化 xini_keyvalue_t 对象。
     *
     * @param [in ] xstr_line  :
     *  用于创建 键值节点 的字符串行，其已经被 trim_xstr() 修剪前后端的空白字符。
     *
     * @param [in ] xowner_ptr : 键值节点的拥有者（xini_section_t 类型）。
     *
     * @return xini_node_t * :
     *  操作成功，返回的 键值节点；若失败，则返回 nullptr 。
     */
    static XiniNodeT *try_create(const std::string &xstr_line, XiniNodeT *xowner_ptr) {
        assert(is_xtrim(xstr_line));
        assert(is_sline(xstr_line));

        if (xstr_line.empty()) {
            return nullptr;
        }

        // 等号位置
        size_t st_eq = xstr_line.find('=');
        if ((0 == st_eq) || (std::string::npos == st_eq)) {
            return nullptr;
        }

        // 键名
        std::string xstr_kname = TrimXstr(xstr_line.substr(0, st_eq));
        if (!CheckKname(xstr_kname)) {
            return nullptr;
        }

        //======================================

        auto *xnode_ptr = new XiniKeyvalueT(xowner_ptr);

        xnode_ptr->m_xstr_kname = xstr_kname;
        xnode_ptr->m_xstr_value = TrimXstr(xstr_line.substr(st_eq + 1));

        //======================================

        return xnode_ptr;
    }

    // construcor/destructor
    explicit XiniKeyvalueT(XiniNodeT *xowner_ptr) : XiniNodeT(kXiniNtypeKeyvalue, xowner_ptr) {}

    ~XiniKeyvalueT() override = default;

    // overrides
   public:
    /**********************************************************/
    /**
     * @brief 将 节点信息 导向 输出流。
     */
    const XiniNodeT &operator>>(std::ostream &ostr) const override {
        ostr << m_xstr_kname << '=' << m_xstr_value << std::endl;
        return *this;
    }

    // template<> functions, for operators
   protected:
    /**********************************************************/
    /**
     * @brief 数值的读操作。
     */
    template <typename NumberType>
    NumberType GetNumb() const {
        NumberType numb;
        std::istringstream istr(m_xstr_value);
        istr >> numb;
        if (istr.fail()) return static_cast<NumberType>(0);
        return numb;
    }

    /**********************************************************/
    /**
     * @brief 数值的读操作（带默认值）。
     */
    template <typename NumberType>
    NumberType GetNumb(NumberType x_default) const {
        if (Empty()) return x_default;

        NumberType numb;
        std::istringstream istr(m_xstr_value);
        istr >> numb;
        if (istr.fail()) return x_default;
        return numb;
    }

    /**********************************************************/
    /**
     * @brief 数值的写操作。
     */
    template <typename NumberType>
    void SetNumb(NumberType x_value) {
        std::ostringstream ostr;
        ostr << x_value;
        assert(!ostr.fail());
        InvkSetValue(ostr.str());
    }

    /**********************************************************/
    /**
     * @brief 数值的写操作。
     */
    template <typename NumberType>
    void SetNumb(NumberType x_value, std::streamsize x_precision) {
        std::ostringstream ostr;
        ostr.precision(x_precision);
        ostr << x_value;
        assert(!ostr.fail());
        InvkSetValue(ostr.str());
    }

    /**********************************************************/
    /**
     * @brief 数值的读操作（键值为 空（或格式非法）时，同步写入默认值）。
     */
    template <typename NumberType>
    NumberType TryNumb(NumberType x_default) {
        if (Empty()) {
            SetNumb(x_default);
            return x_default;
        }

        NumberType numb;
        std::istringstream istr(m_xstr_value);
        istr >> numb;
        if (istr.fail()) {
            SetNumb(x_default);
            return x_default;
        }

        return numb;
    }

    /**********************************************************/
    /**
     * @brief 数值的读操作（键值为 空（或格式非法）时，同步写入默认值）。
     */
    template <typename NumberType>
    NumberType TryNumb(NumberType x_default, std::streamsize x_precision) {
        if (Empty()) {
            SetNumb(x_default, x_precision);
            return x_default;
        }

        NumberType numb;
        std::istringstream istr(m_xstr_value);
        istr >> numb;
        if (istr.fail()) {
            SetNumb(x_default, x_precision);
            return x_default;
        }

        return numb;
    }

    /**********************************************************/
    /**
     * @brief bool 值的读操作（键值为 空（或格式非法）时，同步写入默认值）。
     */
    bool TryBool(bool x_default) {
        //======================================
        // 按 字符串 解析

        if (Empty()) {
            InvkSetValue(std::string(x_default ? "true" : "false"));
            return x_default;
        }

        if (0 == XstrIcmp(m_xstr_value.c_str(), "true")) return true;
        if (0 == XstrIcmp(m_xstr_value.c_str(), "false")) return false;

        //======================================
        // 按 整数值 解析

        long numb;
        std::istringstream istr(m_xstr_value);
        istr >> numb;
        if (istr.fail()) {
            InvkSetValue(std::string(x_default ? "true" : "false"));
            return x_default;
        }

        InvkSetValue(std::string((0L != numb) ? "true" : "false"));
        return (0L != numb);

        //======================================
    }

    // operators
   public:
    //======================================
    // 基础数据类型的读操作

    operator const char *() const { return m_xstr_value.c_str(); }

    operator bool() const {
        if (0 == XstrIcmp(m_xstr_value.c_str(), "true")) return true;
        if (0 == XstrIcmp(m_xstr_value.c_str(), "false")) return false;
        return (0L != GetNumb<long>());
    }

    operator short() const { return GetNumb<short>(); }
    operator unsigned short() const { return GetNumb<unsigned short>(); }
    operator int() const { return GetNumb<int>(); }
    operator unsigned int() const { return GetNumb<unsigned int>(); }
    operator long() const { return GetNumb<long>(); }
    operator unsigned long() const { return GetNumb<unsigned long>(); }
    operator long long() const { return GetNumb<long long>(); }
    operator unsigned long long() const { return GetNumb<unsigned long long>(); }
    operator float() const { return GetNumb<float>(); }
    operator double() const { return GetNumb<double>(); }
    operator long double() const { return GetNumb<long double>(); }

    //======================================
    // 重载 operator ()，实现带上默认值的读操作

    const char *operator()(const char *x_default) const {
        if (Empty()) return x_default;
        return m_xstr_value.c_str();
    }

    bool operator()(bool x_default) const {
        if (0 == XstrIcmp(m_xstr_value.c_str(), "true")) return true;
        if (0 == XstrIcmp(m_xstr_value.c_str(), "false")) return false;
        return (0 != GetNumb<int>(x_default ? 1 : 0));
    }

    short operator()(short x_default) const { return GetNumb<short>(x_default); }
    unsigned short operator()(unsigned short x_default) const {
        return GetNumb<unsigned short>(x_default);
    }
    int operator()(int x_default) const { return GetNumb<int>(x_default); }
    unsigned int operator()(unsigned int x_default) const {
        return GetNumb<unsigned int>(x_default);
    }
    long operator()(long x_default) const { return GetNumb<long>(x_default); }
    unsigned long operator()(unsigned long x_default) const {
        return GetNumb<unsigned long>(x_default);
    }
    long long operator()(long long x_default) const { return GetNumb<long long>(x_default); }
    unsigned long long operator()(unsigned long long x_default) const {
        return GetNumb<unsigned long long>(x_default);
    }
    float operator()(float x_default) const { return GetNumb<float>(x_default); }
    double operator()(double x_default) const { return GetNumb<double>(x_default); }
    long double operator()(long double x_default) const { return GetNumb<long double>(x_default); }

    const char *operator()(const std::string &x_default) const {
        return this->operator()(x_default.c_str());
    }

    //======================================
    // 与重载的 operator () 带默认值读取操作符功能类似，
    // 但键值为 空（或格式非法）时，会同步写入默认值

    const char *TryValue(const char *x_default) {
        if (Empty()) SetValue(x_default);
        return m_xstr_value.c_str();
    }

    bool TryValue(bool x_default) { return TryBool(x_default); }

    short TryValue(short x_default) { return TryNumb<short>(x_default); }
    unsigned short TryValue(unsigned short x_default) { return TryNumb<unsigned short>(x_default); }
    int TryValue(int x_default) { return TryNumb<int>(x_default); }
    unsigned int TryValue(unsigned int x_default) { return TryNumb<unsigned int>(x_default); }
    long TryValue(long x_default) { return TryNumb<long>(x_default); }
    unsigned long TryValue(unsigned long x_default) { return TryNumb<unsigned long>(x_default); }
    long long TryValue(long long x_default) { return TryNumb<long long>(x_default); }
    unsigned long long TryValue(unsigned long long x_default) {
        return TryNumb<unsigned long long>(x_default);
    }
    float TryValue(float x_default) { return TryNumb<float>(x_default, 6); }
    double TryValue(double x_default) { return TryNumb<double>(x_default, 16); }
    long double TryValue(long double x_default) { return TryNumb<long double>(x_default, 16); }

    const char *TryValue(const std::string &x_default) { return this->TryValue(x_default.c_str()); }

    //======================================
    // 基础数据类型的写操作

    XiniKeyvalueT &operator=(const char *x_value) {
        SetValue(std::string(x_value));
        return *this;
    }
    XiniKeyvalueT &operator=(bool x_value) {
        InvkSetValue(std::string(x_value ? "true" : "false"));
        return *this;
    }
    XiniKeyvalueT &operator=(short x_value) {
        SetNumb<short>(x_value);
        return *this;
    }
    XiniKeyvalueT &operator=(unsigned short x_value) {
        SetNumb<unsigned short>(x_value);
        return *this;
    }
    XiniKeyvalueT &operator=(int x_value) {
        SetNumb<int>(x_value);
        return *this;
    }
    XiniKeyvalueT &operator=(unsigned int x_value) {
        SetNumb<unsigned int>(x_value);
        return *this;
    }
    XiniKeyvalueT &operator=(long x_value) {
        SetNumb<long>(x_value);
        return *this;
    }
    XiniKeyvalueT &operator=(unsigned long x_value) {
        SetNumb<unsigned long>(x_value);
        return *this;
    }
    XiniKeyvalueT &operator=(long long x_value) {
        SetNumb<long long>(x_value);
        return *this;
    }
    XiniKeyvalueT &operator=(unsigned long long x_value) {
        SetNumb<unsigned long long>(x_value);
        return *this;
    }
    XiniKeyvalueT &operator=(float x_value) {
        SetNumb<float>(x_value, 6);
        return *this;
    }
    XiniKeyvalueT &operator=(double x_value) {
        SetNumb<double>(x_value, 16);
        return *this;
    }
    XiniKeyvalueT &operator=(long double x_value) {
        SetNumb<long double>(x_value, 16);
        return *this;
    }

    XiniKeyvalueT &operator=(const std::string &x_value) {
        SetValue(x_value);
        return *this;
    }

    /**********************************************************/
    /**
     * @brief 键值节点相互赋值时，只 取值 而 忽略 键名。
     */
    XiniKeyvalueT &operator=(const XiniKeyvalueT &x_value) {
        if (this != &x_value) InvkSetValue(x_value.value());
        return *this;
    }

    //======================================

    // public interfaces
   public:
    /**********************************************************/
    /**
     * @brief 键名。
     */
    inline const std::string &key(void) const { return m_xstr_kname; }

    /**********************************************************/
    /**
     * @brief 键值。
     */
    inline const std::string &value(void) const { return m_xstr_value; }

    /**********************************************************/
    /**
     * @brief 判断 键值 是否为 空。
     */
    bool Empty() const { return m_xstr_value.empty(); }

    /**********************************************************/
    /**
     * @brief 修改键名。
     */
    bool SetKey(const std::string &xstr_key) {
        std::string xstr_kname = TrimXstr(xstr_key);
        if (CheckKname(xstr_kname)) {
            return false;
        }

        return GetOwner()->RenameNsub(this, xstr_kname);
    }

    /**********************************************************/
    /**
     * @brief 设置键值。
     */
    void SetValue(const std::string &x_value) {
        std::string xstr = x_value.substr(0, x_value.find_first_of("\r\n"));
        InvkSetValue(TrimXstr(xstr));
    }

    // inner invoking
   protected:
    /**********************************************************/
    /**
     * @brief 设置（单行文本 且 去除头尾空白字符 的）键值。
     */
    void InvkSetValue(const std::string &xstr_value) {
        if (xstr_value != m_xstr_value) {
            m_xstr_value = xstr_value;
            SetDirty(true);
        }
    }

    std::string m_xstr_kname;  ///< 键名
    std::string m_xstr_value;  ///< 键值
};

////////////////////////////////////////////////////////////////////////////////
// xini_section_t

/**
 * @class xini_section_t
 * @brief INI 文件中的 分节 节点类。
 */
class XiniSectionT : public XiniNodeT {
    friend class XiniFileT;
    friend class XiniKeyvalueT;

    // common data types
   protected:
    typedef std::list<XiniNodeT *> xlst_node_t;
    typedef std::map<std::string, XiniKeyvalueT *, XstrIcmpT> xmap_ndkv_t;

   public:
    typedef xlst_node_t::iterator iterator;
    typedef xlst_node_t::const_iterator const_iterator;

    // common invoking
   protected:
    /**********************************************************/
    /**
     * @brief 修剪 分节名字符串 前后端多余的字符。
     */
    static inline std::string trim_sname(const std::string &xstr_name) {
        return TrimXstr(xstr_name, "[] \t\n\r\f\v");
    }

    /**********************************************************/
    /**
     * @brief 检查分节名字符串格式是否有效。
     *
     * @param [in ] xstr_name :
     *  待检查的 分节名，操作前其已经被
     *  trim_sname() 修剪过前后端多余的字符。
     */
    static inline bool check_sname(const std::string &xstr_name) {
        assert(is_xtrim(xstr_name));
        return IsSline(xstr_name);
    }

    /**********************************************************/
    /**
     * @brief 尝试使用字符串直接创建并初始化 xini_section_t 对象。
     */
    static XiniNodeT *try_create(const std::string &xstr_line, XiniNodeT *xowner_ptr) {
        assert(is_xtrim(xstr_line));
        assert(is_sline(xstr_line));

        //======================================

        if (xstr_line.empty()) {
            return nullptr;
        }

        if ('[' != xstr_line.at(0)) {
            return nullptr;
        }

        std::string::size_type st_pos = xstr_line.find(']', 1);
        if (std::string::npos == st_pos) {
            return nullptr;
        }

        //======================================

        XiniSectionT *xnode_ptr = new XiniSectionT(xowner_ptr);
        xnode_ptr->m_xstr_name = TrimXstr(xstr_line.substr(1, st_pos - 1));

        // 将 自身 作为 节点 加入到 m_xlst_node 中，但并不意味着 m_xlst_node
        // 的 首个节点 就一定是 自身节点，因为 xini_file_t 在加载过程中，
        // 会调用 pop_tail_comment() 操作，这有可能在 m_xlst_node 前端新增
        // 一些 注释/空行节点。所以在进行 流输出 操作时，自身节点 则可起到 占位行
        // 的作用，详细过程可参看 operator >> 的实现流程
        xnode_ptr->m_xlst_node.push_back(xnode_ptr);

        return xnode_ptr;
    }

    // construcor/destructor
   protected:
    XiniSectionT(XiniNodeT *xowner_ptr) : XiniNodeT(kXiniNtypeSection, xowner_ptr) {}

    virtual ~XiniSectionT(void) {
        for (std::list<XiniNodeT *>::iterator itlst = m_xlst_node.begin();
             itlst != m_xlst_node.end(); ++itlst) {
            if (kXiniNtypeSection != (*itlst)->Ntype()) {
                delete (*itlst);
            }
        }

        m_xlst_node.clear();
        m_xmap_ndkv.clear();
    }

    // overrides
   public:
    /**********************************************************/
    /**
     * @brief 将 节点信息 导向 输出流。
     */
    virtual const XiniNodeT &operator>>(std::ostream &ostr) const {
        for (std::list<XiniNodeT *>::const_iterator itlst = m_xlst_node.begin();
             itlst != m_xlst_node.end(); ++itlst) {
            if (this == static_cast<XiniSectionT *>(const_cast<XiniNodeT *>(*itlst))) {
                if (!m_xstr_name.empty()) {
                    ostr << "[" << m_xstr_name << "]" << std::endl;
                }
            } else {
                **itlst >> ostr;
            }
        }

        return *this;
    }

   protected:
    /**********************************************************/
    /**
     * @brief 重命名附属的子节点（键值节点）的索引名。
     * @note  该接口仅由 xini_keyvalue_t::set_key() 调用。
     */
    virtual bool RenameNsub(XiniNodeT *xnsub_ptr, const std::string &xstr_name) {
        assert(XINI_NTYPE_KEYVALUE == xnsub_ptr->ntype());

        return rename_knode(static_cast<XiniKeyvalueT *>(xnsub_ptr), xstr_name);
    }

    // overrides : operator
   public:
    /**********************************************************/
    /**
     * @brief 重载 operator [] 操作符，实现 键值 节点的索引操作。
     */
    XiniKeyvalueT &operator[](const std::string &xstr_key) {
        //======================================

        std::string xstr_nkey = TrimXstr(xstr_key);
        assert(xini_keyvalue_t::check_kname(xstr_nkey));

        //======================================

        XiniKeyvalueT *xndkv_ptr = find_knode(xstr_nkey);
        if (nullptr != xndkv_ptr) {
            return *xndkv_ptr;
        }

        //======================================
        // 若索引的 键值节点 并未在节点表中，
        // 则 新增 此 键值节点，但并不设置 脏标识，
        // 避免存储不必要的 空键值节点

        xndkv_ptr =
            static_cast<XiniKeyvalueT *>(XiniKeyvalueT::try_create(xstr_nkey + "=", GetOwner()));
        assert(nullptr != xndkv_ptr);

        m_xlst_node.push_back(xndkv_ptr);
        m_xmap_ndkv.insert(std::make_pair(xstr_nkey, xndkv_ptr));

        //======================================

        return *xndkv_ptr;
    }

    // public interfaces
   public:
    /**********************************************************/
    /**
     * @brief 分节 名称。
     */
    inline const std::string &name(void) const { return m_xstr_name; }

    /**********************************************************/
    /**
     * @brief 修改 分节 名称。
     */
    bool set_name(const std::string &xstr_name) {
        std::string xstr_sname = trim_sname(xstr_name);
        if (!check_sname(xstr_sname)) {
            return false;
        }

        return GetOwner()->RenameNsub(this, xstr_sname);
    }

    /**********************************************************/
    /**
     * @brief 分节 内的节点数量。
     */
    inline size_t size(void) const { return m_xlst_node.size(); }

    /**********************************************************/
    /**
     * @brief 分节 是否为空。
     */
    inline bool empty() const { return m_xlst_node.empty(); }

    /**********************************************************/
    /**
     * @brief 判断当前分节是否以空行结尾。
     */
    inline bool has_end_nilline(void) const {
        if (!m_xlst_node.empty() && (kXiniNtypeNilline == m_xlst_node.back()->Ntype())) {
            return true;
        }
        return false;
    }

    /**********************************************************/
    /**
     * @brief 判定当前是否已经包含指定的 键值节点。
     */
    inline bool key_included(const std::string &xstr_key) const {
        return (nullptr != find_knode(TrimXstr(xstr_key)));
    }

    /**********************************************************/
    /**
     * @brief 对 键值节点 进行重命名（索引键名）操作。
     *
     * @param [in ] xstr_key  : 目标操作的索引键名。
     * @param [in ] xstr_name : 重新设置键值节点的索引键名。
     *
     * @return 重命名操作 是否成功。
     */
    bool key_rename(const std::string &xstr_key, const std::string &xstr_name) {
        //======================================

        XiniKeyvalueT *xndkv_ptr = find_knode(TrimXstr(xstr_key));
        if (nullptr == xndkv_ptr) {
            return false;
        }

        //======================================

        std::string xstr_kname = TrimXstr(xstr_name);
        if (!XiniKeyvalueT::CheckKname(xstr_kname)) {
            return false;
        }

        return rename_knode(xndkv_ptr, xstr_kname);
    }

    /**********************************************************/
    /**
     * @brief 删除指定键值。
     */
    bool key_remove(const std::string &xstr_key) {
        //======================================

        xmap_ndkv_t::iterator itmap = m_xmap_ndkv.find(TrimXstr(xstr_key));
        if (itmap == m_xmap_ndkv.end()) {
            return false;
        }

        //======================================

        for (xlst_node_t::iterator itlst = m_xlst_node.begin(); itlst != m_xlst_node.end();
             ++itlst) {
            if (kXiniNtypeKeyvalue != (*itlst)->Ntype()) continue;

            if (static_cast<XiniNodeT *>(itmap->second) == (*itlst)) {
                delete *itlst;
                m_xlst_node.erase(itlst);

                break;
            }
        }

        m_xmap_ndkv.erase(itmap);

        SetDirty(true);

        //======================================

        return true;
    }

    // iterator
   public:
    /**********************************************************/
    /**
     * @brief 节点表的起始位置迭代器。
     */
    inline iterator begin(void) { return m_xlst_node.begin(); }

    /**********************************************************/
    /**
     * @brief 节点表的起始位置迭代器。
     */
    inline const_iterator begin(void) const { return m_xlst_node.begin(); }

    /**********************************************************/
    /**
     * @brief 节点表的结束位置迭代器。
     */
    inline iterator end(void) { return m_xlst_node.end(); }

    /**********************************************************/
    /**
     * @brief 节点表的结束位置迭代器。
     */
    inline const_iterator end(void) const { return m_xlst_node.end(); }

    /**********************************************************/
    /**
     * @brief 返回节点表中 首个 键值节点 的迭代器。
     */
    inline iterator begin_kv(void) {
        iterator xiter = m_xlst_node.begin();
        if (kXiniNtypeKeyvalue == (*xiter)->Ntype()) return xiter;
        return next_kv(xiter);
    }

    /**********************************************************/
    /**
     * @brief 返回节点表中 首个 键值节点 的迭代器。
     */
    inline const_iterator begin_kv(void) const {
        const_iterator xiter = m_xlst_node.begin();
        if (kXiniNtypeKeyvalue == (*xiter)->Ntype()) return xiter;
        return next_kv(xiter);
    }

    /**********************************************************/
    /**
     * @brief 返回 下一个 键值节点 的迭代器。
     */
    iterator next_kv(iterator xiter) {
        const iterator xiter_end = m_xlst_node.end();
        if (xiter != xiter_end) {
            while (++xiter != xiter_end)
                if (kXiniNtypeKeyvalue == (*xiter)->Ntype()) return xiter;
        }

        return xiter_end;
    }

    /**********************************************************/
    /**
     * @brief 返回 下一个 键值节点 的迭代器。
     */
    const_iterator next_kv(const_iterator xiter) const {
        const const_iterator xiter_end = m_xlst_node.end();
        if (xiter != xiter_end) {
            while (++xiter != xiter_end)
                if (kXiniNtypeKeyvalue == (*xiter)->Ntype()) return xiter;
        }

        return xiter_end;
    }

    // inner invoking
   protected:
    /**********************************************************/
    /**
     * @brief 添加（空行、注释、键值 类型的）节点。
     *
     * @param [in ] xnode_ptr: （空行、注释、键值 类型的）节点。
     *
     * @return 操作是否成功。
     */
    bool push_node(XiniNodeT *xnode_ptr) {
        if (nullptr == xnode_ptr) {
            return false;
        }

        if ((kXiniNtypeNilline == xnode_ptr->Ntype()) ||
            (kXiniNtypeComment == xnode_ptr->Ntype())) {
            m_xlst_node.push_back(xnode_ptr);
            return true;
        }

        if (kXiniNtypeKeyvalue == xnode_ptr->Ntype()) {
            XiniKeyvalueT *xnode_kvptr = static_cast<XiniKeyvalueT *>(xnode_ptr);

            if (nullptr != find_knode(xnode_kvptr->key())) {
                return false;
            }

            m_xlst_node.push_back(xnode_ptr);
            m_xmap_ndkv.insert(std::make_pair(xnode_kvptr->key(), xnode_kvptr));
            return true;
        }

        return false;
    }

    /**********************************************************/
    /**
     * @brief 查找分节下的 键值 节点。
     *
     * @param [in ] xstr_xkey: 索引键字符串，比较时忽略大小写。
     *
     * @return xini_keyvalue_t *
     *         - 成功，返回 对应的节点；
     *         - 失败，返回 nullptr 。
     */
    XiniKeyvalueT *find_knode(const std::string &xstr_xkey) const {
        xmap_ndkv_t::const_iterator itfind = m_xmap_ndkv.find(xstr_xkey);
        if (itfind != m_xmap_ndkv.end()) {
            return itfind->second;
        }

        return nullptr;
    }

    /**********************************************************/
    /**
     * @brief 从 节点表 尾部取出 非当前 分节 下的注释节点（按 空行 节点作为分界）。
     *
     * @param [in ] xlst_comm : 接收返回的注释节点表（在链表头部添加返回的节点）。
     * @param [in ] xbt_front : 表明操作是从 xlst_comm 前/后附加返回的节点。
     *
     * @return size_t
     *         - 返回取出的节点数量。
     */
    size_t pop_tail_comment(std::list<XiniNodeT *> &xlst_comm, bool xbt_front) {
        std::list<XiniNodeT *> xlst_node;

        size_t xst_line = 0;
        size_t xst_maxl = m_xlst_node.size();

        // 节点表只有三种类型的节点：键值，空行，注释，
        // 以及 另外加上 自身的 分节节点

        while ((xst_line++ < xst_maxl) && !m_xlst_node.empty()) {
            XiniNodeT *xnode_ptr = m_xlst_node.back();

            // 遇到空行节点
            if (kXiniNtypeNilline == xnode_ptr->Ntype()) {
                if (xst_line > 1) break;

                // 只容许第一个是空行
                xlst_node.push_front(xnode_ptr);
                m_xlst_node.pop_back();
                continue;
            }

            // 若反向遍历过程中，一直未遇到空行，
            // 则将原取出的注释节点还回节点表中
            if ((kXiniNtypeKeyvalue == xnode_ptr->Ntype()) ||
                (kXiniNtypeSection == xnode_ptr->Ntype())) {
                m_xlst_node.splice(m_xlst_node.end(), xlst_node);
                break;
            }

            if (kXiniNtypeComment == xnode_ptr->Ntype()) {
                xlst_node.push_front(xnode_ptr);
                m_xlst_node.pop_back();
            } else {
                // 未识别的节点类型
                assert(false);
            }
        }

        size_t xst_count = xlst_node.size();
        if (xst_count > 0) {
            // 设置返回结果
            if (xbt_front) {
                xlst_node.splice(xlst_node.end(), xlst_comm);
                xlst_comm.swap(xlst_node);
            } else {
                xlst_comm.splice(xlst_comm.end(), xlst_node);
            }
        }

        return xst_count;
    }

    /**********************************************************/
    /**
     * @brief 对 键值节点 进行重命名操作。
     *
     * @param [in ] xndkv_ptr : 目标操作的键值节点。
     * @param [in ] xstr_name : 重新设置键值节点的索引键名。
     *
     * @return 重命名操作 是否成功。
     */
    bool rename_knode(XiniKeyvalueT *xndkv_ptr, const std::string &xstr_name) {
        //======================================

        // 与键值节点原有名称一致，忽略后续操作
        if (0 == XstrIcmp(xndkv_ptr->key().c_str(), xstr_name.c_str())) {
            return true;
        }

        // 判定所要设置的键值节点名称，
        // 与节点表中的其他键值节点名称 是否重名
        if (nullptr != find_knode(xstr_name)) {
            return false;
        }

        //======================================
        // 先从映射表中移除旧有的键值节点映射，
        // 再对键值节点进行重命名，最后重新加入到映射表中

        m_xmap_ndkv.erase(xndkv_ptr->key());
        xndkv_ptr->m_xstr_kname = xstr_name;
        m_xmap_ndkv.insert(std::make_pair(xndkv_ptr->key(), xndkv_ptr));

        SetDirty(true);

        //======================================

        return true;
    }

   protected:
    std::string m_xstr_name;  ///< 分节名称
    xlst_node_t m_xlst_node;  ///< 分节下的节点表
    xmap_ndkv_t m_xmap_ndkv;  ///< 分节下的 键值节点 映射表
};

////////////////////////////////////////////////////////////////////////////////
// xini_file_t

/**
 * @class xini_file_t
 * @brief INI 文件操作类。
 */
class XiniFileT : public XiniNodeT {
    friend class XiniSectionT;

    // common data types
   protected:
    typedef std::list<XiniSectionT *> xlst_section_t;
    typedef std::map<std::string, XiniSectionT *, XstrIcmpT> xmap_section_t;

   public:
    typedef xlst_section_t::iterator iterator;
    typedef xlst_section_t::const_iterator const_iterator;

    // common invoking
   protected:
    /**********************************************************/
    /**
     * @brief 依据给定的 INI 文本行，创建相应的节点。
     */
    static XiniNodeT *make_node(const std::string &xstr_line, XiniFileT *xowner_ptr) {
        XiniNodeT *xnode_ptr = nullptr;

#define XTRY_CREATE(nptr, node, owner)             \
    do {                                           \
        nptr = node::try_create(xstr_line, owner); \
        if (nullptr != nptr) return nptr;          \
    } while (0)

        XTRY_CREATE(xnode_ptr, XiniNillineT, xowner_ptr);
        XTRY_CREATE(xnode_ptr, XiniCommentT, xowner_ptr);
        XTRY_CREATE(xnode_ptr, XiniSectionT, xowner_ptr);
        XTRY_CREATE(xnode_ptr, XiniKeyvalueT, xowner_ptr);

#undef XTRY_CREATE

        return xnode_ptr;
    }

    // constructor/destructor
   public:
    XiniFileT(void) : XiniNodeT(kXiniNtypeFileroot, nullptr), m_xbt_dirty_(false) {}

    XiniFileT(const std::string &xstr_filepath)
        : XiniNodeT(kXiniNtypeFileroot, nullptr), m_xbt_dirty_(false) {
        Load(xstr_filepath);
    }

    virtual ~XiniFileT(void) { Release(); }

    // overrides
   public:
    /**********************************************************/
    /**
     * @brief 将 节点信息 导向 输出流。
     */
    virtual const XiniNodeT &operator>>(std::ostream &ostr) const {
        for (std::list<XiniSectionT *>::const_iterator itlst = m_xlst_sect_.begin();
             itlst != m_xlst_sect_.end(); ++itlst) {
            if ((*itlst)->empty()) continue;

            **itlst >> ostr;
            if (!(*itlst)->has_end_nilline() && ((*itlst) != m_xlst_sect_.back())) {
                ostr << std::endl;
            }
        }

        return *this;
    }

    /**********************************************************/
    /**
     * @brief 脏标识。
     */
    bool IsDirty() const override { return m_xbt_dirty_; }

    /**********************************************************/
    /**
     * @brief 设置脏标识。
     */
    void SetDirty(bool x_dirty) override { m_xbt_dirty_ = x_dirty; }

   protected:
    /**********************************************************/
    /**
     * @brief 重命名附属的子节点（分节节点）的索引名。
     * @note  该接口仅由 xini_section_t::set_name() 调用。
     */
    bool RenameNsub(XiniNodeT *xnsub_ptr, const std::string &xstr_name) override {
        assert(XINI_NTYPE_SECTION == xnsub_ptr->ntype());

        return RenameSect(static_cast<XiniSectionT *>(xnsub_ptr), xstr_name);
    }

    // overrides : operator
   public:
    /**********************************************************/
    /**
     * @brief 从 输出流 构建 xini_file_t 内容。
     */
    XiniFileT &operator<<(std::istream &istr) {
        //======================================

        // 记录当前操作的分节
        XiniSectionT *xsect_ptr = nullptr;

        if (m_xlst_sect_.empty()) {
            // 当前分节表为空，则创建一个空分节名的 分节 节点
            xsect_ptr = new XiniSectionT(this);
            m_xlst_sect_.push_back(xsect_ptr);

            assert(m_xmap_sect.empty());
            m_xmap_sect_.insert(std::make_pair(std::string(""), xsect_ptr));
        } else {
            // 取尾部分节作为当前操作的 分节 节点
            xsect_ptr = m_xlst_sect_.back();

            // 确保尾部分节空行结尾
            if (!xsect_ptr->has_end_nilline()) {
                xsect_ptr->push_node(new XiniNillineT(this));
            }
        }

        //======================================

        // 逐行解析 INI 文件，构建节点表
        while (!istr.eof()) {
            //======================================
            // 读取文本行

            std::string xstr_line;
            std::getline(istr, xstr_line);
            xstr_line = TrimXstr(xstr_line);

            // 最后一个空行不放到节点表中，避免文件关闭时 持续增加 尾部空行
            if (istr.eof() && xstr_line.empty()) {
                break;
            }

            //======================================

            // 创建节点
            XiniNodeT *xnode_ptr = make_node(xstr_line, this);
            if (nullptr == xnode_ptr) {
                continue;
            }

            // 若为 分节 节点，则加入到分节表中，并更新当前操作的 分节节点
            if (kXiniNtypeSection == xnode_ptr->Ntype()) {
                xsect_ptr = PushSect(static_cast<XiniSectionT *>(xnode_ptr), xsect_ptr);

                if (xsect_ptr != static_cast<XiniSectionT *>(xnode_ptr))
                    delete xnode_ptr;  // 添加新分节失败，删除该节点
                else
                    SetDirty(true);  // 添加新分节成功，设置脏标识

                continue;
            }

            // 加入 当前分节
            if (xsect_ptr->push_node(xnode_ptr)) {
                SetDirty(true);
            } else {
                // 加入分节失败，可能是因为：
                // 其为 键值 节点，与 分节 节点表中已有的 节点 索引键 冲突
                delete xnode_ptr;
            }

            //======================================
        }

        //======================================

        return *this;
    }

    /**********************************************************/
    /**
     * @brief 重载 operator [] 操作符，实现 分节 索引操作。
     */
    XiniSectionT &operator[](const std::string &xstr_sect) {
        //======================================

        std::string xstr_name = XiniSectionT::trim_sname(xstr_sect);
        assert(xini_section_t::check_sname(xstr_name));

        //======================================

        XiniSectionT *xsect_ptr = FindSect(xstr_name);
        if (nullptr != xsect_ptr) {
            return *xsect_ptr;
        }

        //======================================
        // 若索引的分节并未在 分节 的节点表中，
        // 则 新增 此分节，但并不设置 脏标识，
        // 避免存储不必要的  空分节

        xsect_ptr =
            static_cast<XiniSectionT *>(XiniSectionT::try_create("[" + xstr_name + "]", this));
        assert(nullptr != xsect_ptr);

        m_xlst_sect_.push_back(xsect_ptr);
        m_xmap_sect_.insert(std::make_pair(xstr_name, xsect_ptr));

        //======================================

        return *xsect_ptr;
    }

    // public interfaces
   public:
    /**********************************************************/
    /**
     * @brief 从指定路径的文件中加载 INI 内容。
     * @note
     *  load() 操作的成功与否，并不影响后续的键值读写操作，
     *  其只能标示 xini_file_t 对象是否关联可至指定路径
     *  （本地磁盘 或 远程网络 等的）文件。
     *
     * @param [in ] xstr_text : 文件路径。
     *
     * @return bool
     *         - 成功，返回 true ；
     *         - 失败，返回 false。
     */
    bool Load(const std::string &xstr_filepath) {
        // 先释放当前对象
        Release();

        // 不管后续操作是否成功，都关联到新指定的 INI 文件路径
        m_xstr_path_ = xstr_filepath;

        if (xstr_filepath.empty()) {
            return false;
        }

        // 打开文件
        std::ifstream xfile_reader(xstr_filepath.c_str());
        if (!xfile_reader.is_open()) {
            return false;
        }

        // 跳过字符流的头部编码信息（如 utf-8 的 bom 标识）
        while (!xfile_reader.eof()) {
            int xchar = xfile_reader.get();
            if (std::iscntrl(xchar) || std::isprint(xchar)) {
                xfile_reader.putback(static_cast<char>(xchar));
                break;
            }

            m_xstr_head_.push_back(static_cast<char>(xchar));
        }

        *this << xfile_reader;
        SetDirty(false);

        return true;
    }

    /**********************************************************/
    /**
     * @brief 将当前文件根下的所有节点直接输出到文件中。
     */
    bool Dump(const std::string &xstr_filepath) {
        // 打开文件
        std::ofstream xfile_writer(xstr_filepath.c_str(), std::ios_base::trunc);
        if (!xfile_writer.is_open()) {
            return false;
        }

        if (!m_xstr_head_.empty()) xfile_writer << m_xstr_head_.c_str();
        *this >> xfile_writer;

        return true;
    }

    /**********************************************************/
    /**
     * @brief 释放对象资源（可以不显示调用，对象析构函数中会自动调用该接口）。
     */
    void Release() {
        if (IsDirty()) {
            Dump(m_xstr_path_);
            SetDirty(false);
        }
        m_xstr_path_.clear();
        m_xstr_head_.clear();

        for (auto &itlst : m_xlst_sect_) {
            delete itlst;
        }

        m_xlst_sect_.clear();
        m_xmap_sect_.clear();
    }

    /**********************************************************/
    /**
     * @brief 当前关联的文件路径。
     */
    const std::string &Filepath() const { return m_xstr_path_; }

    /**********************************************************/
    /**
     * @brief 返回当前分节数量。
     */
    size_t SectCount() const { return m_xlst_sect_.size(); }

    /**********************************************************/
    /**
     * @brief 判定当前是否包含指定的 分节。
     */
    bool SectIncluded(const std::string &xstr_sect) const {
        return (nullptr != FindSect(XiniSectionT::trim_sname(xstr_sect)));
    }

    /**********************************************************/
    /**
     * @brief 对 分节 进行重命名操作。
     *
     * @param [in ] xstr_sect : 目标操作的分节名称。
     * @param [in ] xstr_name : 重新设置分节的名称。
     *
     * @return 重命名操作 是否成功。
     */
    bool SectRename(const std::string &xstr_sect, const std::string &xstr_name) {
        //======================================

        XiniSectionT *xsect_ptr = FindSect(XiniSectionT::trim_sname(xstr_sect));
        if (nullptr == xsect_ptr) {
            return false;
        }

        //======================================

        std::string xstr_sname = XiniSectionT::trim_sname(xstr_name);
        if (!XiniSectionT::check_sname(xstr_sname)) {
            return false;
        }

        return RenameSect(xsect_ptr, xstr_sname);
    }

    /**********************************************************/
    /**
     * @brief 删除指定分节。
     */
    bool SectRemove(const std::string &xstr_sect) {
        //======================================

        auto itmap = m_xmap_sect_.find(XiniSectionT::trim_sname(xstr_sect));
        if (itmap == m_xmap_sect_.end()) {
            return false;
        }

        //======================================

        for (auto itlst = m_xlst_sect_.begin(); itlst != m_xlst_sect_.end(); ++itlst) {
            if (itmap->second == (*itlst)) {
                delete *itlst;
                m_xlst_sect_.erase(itlst);

                break;
            }
        }

        m_xmap_sect_.erase(itmap);

        SetDirty(true);

        //======================================

        return true;
    }

    // iterator

    /**********************************************************/
    /**
     * @brief 分节表的起始位置迭代器。
     */
    iterator Begin() { return m_xlst_sect_.begin(); }

    /**********************************************************/
    /**
     * @brief 分节表的起始位置迭代器。
     */
    const_iterator Begin() const { return m_xlst_sect_.begin(); }

    /**********************************************************/
    /**
     * @brief 分节表的结束位置迭代器。
     */
    iterator End() { return m_xlst_sect_.end(); }

    /**********************************************************/
    /**
     * @brief 分节表的结束位置迭代器。
     */
    const_iterator End() const { return m_xlst_sect_.end(); }

    // inner invoking
   protected:
    /**********************************************************/
    /**
     * @brief 查找分节。
     */
    XiniSectionT *FindSect(const std::string &xstr_sect) const {
        auto itfind = m_xmap_sect_.find(xstr_sect);
        if (itfind != m_xmap_sect_.end()) {
            return itfind->second;
        }

        return nullptr;
    }

    /**********************************************************/
    /**
     * @brief 加入新分节（该接口仅由 operator << 调用）。
     *
     * @param [in ] xnew_ptr  : 新增分节。
     * @param [in ] xsect_ptr : 当前操作分节。
     *
     * @return xini_section_t *
     *         - 返回当前操作分节。
     *         - 若返回值 != xnew_ptr 则表示操作失败，新增分节和内部分节重名。
     */
    XiniSectionT *PushSect(XiniSectionT *xnew_ptr, XiniSectionT *xsect_ptr) {
        // 查找同名分节
        XiniSectionT *xfind_ptr = FindSect(xnew_ptr->name());

        if (nullptr == xfind_ptr) {
            // 不存在同名分节，则将新增分节加入到节点表尾部
            m_xlst_sect_.push_back(xnew_ptr);
            m_xmap_sect_.insert(std::make_pair(xnew_ptr->name(), xnew_ptr));

            // 将当前操作分节的节点表中的 尾部注释节点，
            // 全部转移到新增分节的节点表前
            xsect_ptr->pop_tail_comment(xnew_ptr->m_xlst_node, true);

            // 将新增分节作为当前操作分节返回
            xsect_ptr = xnew_ptr;
        } else if (xfind_ptr != xsect_ptr) {
            // 将当前操作分节的节点表中的 尾部注释节点，
            // 全部转移到同名分节的节点表后

            // 保证空行隔开
            if (!xfind_ptr->has_end_nilline()) {
                xfind_ptr->push_node(new XiniNillineT(this));
            }

            // 增加注释节点
            xsect_ptr->pop_tail_comment(xfind_ptr->m_xlst_node, false);

            // 保证空行隔开
            if (!xfind_ptr->has_end_nilline()) {
                xfind_ptr->push_node(new XiniNillineT(this));
            }

            // 将同名分节作为当前操作分节返回
            xsect_ptr = xfind_ptr;
        }

        return xsect_ptr;
    }

    /**********************************************************/
    /**
     * @brief 对 分节 进行重命名操作。
     *
     * @param [in ] xsect_ptr : 目标操作的分节。
     * @param [in ] xstr_name : 重新设置分节的名称。
     *
     * @return 重命名操作 是否成功。
     */
    bool RenameSect(XiniSectionT *xsect_ptr, const std::string &xstr_name) {
        //======================================

        // 与分节原有名称一致，忽略后续操作
        if (0 == XstrIcmp(xsect_ptr->name().c_str(), xstr_name.c_str())) {
            return true;
        }

        // 判定所要设置的分节名称，
        // 与分节表中的其他分节名称 是否重名
        if (nullptr != FindSect(xstr_name)) {
            return false;
        }

        //======================================
        // 先从映射表中移除旧有的分节节点映射，
        // 再对分节进行重命名，最后重新加入到映射表中

        m_xmap_sect_.erase(xsect_ptr->name());
        xsect_ptr->m_xstr_name = xstr_name;
        m_xmap_sect_.insert(std::make_pair(xsect_ptr->name(), xsect_ptr));

        SetDirty(true);

        //======================================

        return true;
    }

    // data members

    bool m_xbt_dirty_;            ///< 脏标识
    std::string m_xstr_path_;     ///< 文件路径
    std::string m_xstr_head_;     ///< 用于存储文件头的编码字符信息（如 utf-8 的 bom 标识）
    xlst_section_t m_xlst_sect_;  ///< 文件根下的 分节 节点表
    xmap_section_t m_xmap_sect_;  ///< 各个 分节 的节点映射表
};

/**********************************************************/
/**
 * @brief 定义 xini_file_t 的流输入操作符函数。
 */
inline std::istream &operator>>(std::istream &istr, XiniFileT &xini_file) {
    xini_file << istr;
    return istr;
}
