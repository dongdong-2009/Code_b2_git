#pragma once;

#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include <vector>
#include <map>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

template <class T> class TreeItemCache
{
private:
	struct Data
    {
		boost::shared_ptr<T> userData;
        TreeItem treeItem;
        bool root;
    };

    struct TreeNode
    {
        Data data;
		std::vector<boost::shared_ptr<TreeNode>> childs;
    };
public:
    static const unsigned int INVALID_ID;

    TreeItemCache()
    {
    }

    ~TreeItemCache()
    {
    }

	void insertTreeItem(const TreeItem& item, T* userData, bool root = false)
    {
		boost::shared_ptr<TreeNode> node(new TreeNode);
        node->data.treeItem = item;
		node->data.userData = boost::shared_ptr<T>(userData);
        node->data.root = root;

        m_treeNodeMap.insert(std::make_pair(item.ID, node));
        if(item.ID != item.ParentID
           && m_treeNodeMap.count(item.ParentID) != 0
           && m_treeNodeMap[item.ParentID]->data.treeItem.HasChild)
        {
            m_treeNodeMap[item.ParentID]->childs.push_back(node);
        }
    }

    TreeItem getTreeItem(const unsigned int id)
    {
        TreeItem item = {false, INVALID_ID, INVALID_ID, ""};
        if(m_treeNodeMap.count(id) != 0)
        {
            item = m_treeNodeMap[id]->data.treeItem;
        }
        return item;
    }

    std::vector<TreeItem> getChildItems(const unsigned int parentId)
    {
        std::vector<TreeItem> items;
        if(m_treeNodeMap.count(parentId) != 0 && m_treeNodeMap[parentId]->data.treeItem.HasChild)
        {
			BOOST_FOREACH(const boost::shared_ptr<TreeNode>& i, m_treeNodeMap[parentId]->childs)
            {
                items.push_back(i->data.treeItem);
            }
        }
        return items;
    }

    std::vector<TreeItem> getRootItems()
    {
        std::vector<TreeItem> items;
        BOOST_FOREACH(const TreeNodeMap::value_type & i, m_treeNodeMap)
        {
            const boost::shared_ptr<TreeNode>& treeNode = i.second;
            if(treeNode->data.root)
            {
                items.push_back(treeNode->data.treeItem);
            }
        }
        return items;
    }

    template<class S, class Equal> unsigned int getItemIdByUserData(const S& key, const Equal& equal)
    {
        BOOST_FOREACH(const TreeNodeMap::value_type & i, m_treeNodeMap)
        {
			const boost::shared_ptr<TreeNode>& treeNode = i.second;
            if(treeNode->data.userData != NULL && equal(*(treeNode->data.userData), key))
            {
                return treeNode->data.treeItem.ID;
            }
        }
        return INVALID_ID;
    }

	T* getUserDataByItemId(const unsigned int id)
    {
        if(m_treeNodeMap.count(id) != 0)
        {
            return m_treeNodeMap[id]->data.userData.get();
        }
		return NULL;
    }

    bool hasChild(const unsigned int id)
    {
        return m_treeNodeMap.count(id) != 0 && !(m_treeNodeMap[id]->childs.empty());
    }

	bool isEmpty()
	{
		return m_treeNodeMap.empty();
	}

private:
	typedef std::map<unsigned int, boost::shared_ptr<TreeNode>> TreeNodeMap;
    TreeNodeMap m_treeNodeMap;
};

template <class T> const unsigned int TreeItemCache<T>::INVALID_ID = -1;;