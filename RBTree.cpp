#include <iostream>
using namespace std;

struct RBNode
{
    int key;
    char color;
    RBNode *left, *right, *parent;
    int height;
    
    RBNode(int k)
    {
        key = k;
        left = nullptr;
        right = nullptr;
        parent = nullptr;
        height = 1;
        color = 'R';
    }
};

RBNode* root = nullptr;

// 🔄 Left Rotate
void leftRotate(RBNode* &root, RBNode* x) {
    RBNode* y = x->right;
    x->right = y->left;
    if (y->left != nullptr)
        y->left->parent = x;

    y->parent = x->parent;
    if (x->parent == nullptr)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

// 🔄 Right Rotate
void rightRotate(RBNode* &root, RBNode* y) {
    RBNode* x = y->left;
    y->left = x->right;
    if (x->right != nullptr)
        x->right->parent = y;

    x->parent = y->parent;
    if (y->parent == nullptr)
        root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;

    x->right = y;
    y->parent = x;
}



/*
    fixTree() will fix the RB-Tree properties after insertion:
    Property is only voilated when red-red conflict occurs during insertion, 
    so the fixing loop runs only till parent is red
    if parent is null, then simply color root black

    in whole cases, parent is red ( not null ), as that is the only reason to fix insertion
       
        1: { if uncle is red , not null }
            recoloring at parent level ( both parent, uncle changes from red to black )
            set newnode as grandparent & loop continues
        2: { if uncle is black or null }
            bring grandparent, parent & newnode in same line ( same LL, RR. Not LR or RL )
            rotate grandparent in direction of uncle
            color grandparent as red, parent as black

*/

void fixTree( RBNode *root, RBNode *newnode )
{
    while( newnode->parent != nullptr  &&  newnode->parent->color == 'R' )
    {
        RBNode *grandparent = newnode->parent->parent;

        if( grandparent->left == newnode->parent )
        {
            RBNode *uncle = grandparent->right;

            if( uncle != nullptr  &&  uncle->color == 'R' )
            {
                newnode->parent->color = 'B';
                uncle->color = 'B';
                grandparent->color = 'R';
                newnode = grandparent;
            }

            else
            {
                if( newnode->parent->right == newnode )
                {
                    newnode = newnode->parent;
                    leftRotate(root, newnode);
                }

                rightRotate(root, grandparent);
                grandparent->color = 'R';
                newnode->parent->color = 'B';
            }
        }

        else if( grandparent->right == newnode->parent )
        {
            RBNode *uncle = grandparent->left;

            if( uncle != nullptr  &&  uncle->color == 'R' )
            {
                uncle->color = 'B';
                newnode->parent->color = 'B';
                grandparent->color = 'R';
                newnode = grandparent;
            }

            else
            {
                if( newnode->parent->left == newnode )
                {
                    newnode = newnode->parent;
                    rightRotate(root, newnode);
                }
                leftRotate(root, grandparent);
                grandparent->color = 'R';
                newnode->parent->color = 'B';
            }
        }
    }
root->color = 'B';
}

void insert( RBNode* &root, int key )
{
    RBNode *newnode = new RBNode(key);
    RBNode *prev = nullptr;
    RBNode *temp = root;

    while( temp != nullptr )
    {
        prev = temp;

        if( key < temp->key )
            temp = temp->left;
        else if( key > temp->key )
            temp = temp->right;
        else
            return;
    }

    newnode->parent = prev;

    if( prev == nullptr )
        root = newnode;
    else if( key < prev->key )
        prev->left = newnode;
    else if( key > prev->key )
        prev->right = newnode;

        fixTree(root, newnode);
}




void deleteNode( RBNode* &root, int key )
{
    RBNode *temp = root;

    while(temp->key != key)
    {
        if( key < temp->key )
            temp = temp->left;
        else if( key > temp->key )
            temp = temp->right;
    }

// if node is simple red-leaf, just delete it
    if( temp->left == nullptr && temp->right == nullptr && temp->color == 'R' )
    {
        if( temp->parent->left == temp )
            temp->parent->left = nullptr;
        else if( temp->parent->right == temp )
            temp->parent->right = nullptr;
        delete temp;
    }

// only one child of red node, joint it's parent to child & delete node;
    else if( temp->left == nullptr && temp->color == 'R' )
    {
        if( temp->parent->left == temp )
            temp->parent->left = temp->right;
        else if( temp->parent->right == temp )
            temp->parent->right = temp->right;
        temp->right->parent = temp->parent;
        delete temp;
    }

// same as above case
    else if( temp->right == nullptr && temp->color == 'R' )
    {
        if( temp->parent->right == temp )
            temp->parent->right = temp->left;
        else if( temp->parent->left == temp )
            temp->parent->left = temp->left;
        temp->left->parent = temp->parent;
        delete temp;
    }
 
// both child not null, replace key with it's inorder successor & recurse deletion on successor node
    else if( temp->left != nullptr && temp->right != nullptr && temp->color == 'R' )
    {
        RBNode *succ = temp->right;

        while( succ->left != nullptr )
            succ = succ->left;

        temp->key = succ->key;

        deleteNode(root, succ->key);
    }


    else if( temp->color == 'B' && temp->left == nullptr && temp->right != nullptr && temp->right->color == 'R' )
    {
        if( temp->parent->left == temp )
        {
            temp->parent->left = temp->right;
            temp->parent->left->color = 'B';
        }

        else if( temp->parent->right == temp )
        {
            temp->parent->right = temp->right;
            temp->parent->right->color = 'B';
        }

    }

    else if( temp->color == 'B' && temp->right == nullptr && temp->left != nullptr && temp->left->color == 'R' )
    {
        if( temp->parent->left == temp )
        {
            temp->parent->left = temp->left;
            temp->parent->left->color = 'B';
        }
        else if( temp->parent->right == temp )
        {
            temp->parent->right = temp->left;
            temp->parent->left->color = 'B';
        }
    }


    // Deleting a Black-Leaf node
    else if( temp->color == 'B' && temp->left == nullptr && temp->right == nullptr )
    {

        // node to be deleted is left child of it's parent
        if( temp->parent->left == temp ) 
        { 
            RBNode *sibling = temp->parent->right;

            // if the sibling of node to be deleted is red, then just change colors of parent & rotate parent in direction of child
            if( sibling != nullptr and sibling->color == 'R' )
            {
                temp->parent->color = 'R';
                sibling->color = 'B';
                leftRotate(root, temp->parent);
                deleteNode(root, key);
                return;
            }

            // Sibling is black & both it's children are also black, then color sibling red & recurse deletion again from root
            // valid as black node to be deleted from one side + it's sibling is done red. So same decrement in black height is there
            else if( sibling == nullptr or sibling->color == 'B' )
            {
                if( (sibling->left == nullptr or sibling->left->color == 'B') and ( sibling->right == nullptr or sibling->right->color == 'B' ) )
                {
                    sibling->color = 'R';
                    deleteNode(root, key);  // now 1 black node height is decreased in this path, so we recurse
                    return;
                }

                 // Last case: sibling black, at least one child red
                else
                {
                    if(sibling->right != nullptr && sibling->right->color == 'R')
                    {
                        sibling->right->color = 'B';
                        sibling->color = temp->parent->color;
                        temp->parent->color = 'B';
                        leftRotate(root, temp->parent);
                    }
                
                    else if(sibling->left != nullptr && sibling->left->color == 'R')
                    {
                        sibling->left->color = 'B';
                        rightRotate(root, sibling);
                        leftRotate(root, temp->parent);
                        sibling->color = temp->parent->color;
                        temp->parent->color = 'B';
                    }

                    // finally delete the node
                    temp->parent->left = nullptr;
                    delete temp;
                    return;
                }

            }

            

        }

        // Node to be deleted is right child of it's parent
        else if( temp->parent->right == temp )
        {
            RBNode *sibling = temp->parent->left;

            // if sibling is red, then parent=red & sibling=black & then rotate parent to side of child & recurse deletion from root again
            if( sibling != nullptr and sibling->color == 'R' )
            {
                temp->parent->color = 'R';
                sibling->color = 'B';
                rightRotate(root, temp->parent);
                deleteNode(root, key);
                return;
            }


            else if( sibling == nullptr or sibling->color == 'B' )
            {
                if( (sibling->left == nullptr or sibling->left->color == 'B') and (sibling->right == nullptr or sibling->right->color == 'B') )
                {
                    sibling->color = 'R';
                    deleteNode(root, key);
                    return;
                }
/*
R = child of sibling which is red
R ko black kiya, us direction me 1 blach height badhi, then rotate kiya, 
phir vo black heigh increment doosri side ( node to be deleted side ) chala gaya, 
so ab dono side barabar krne ke liye node to be deleted hi hata di
*/
                else
                {
                    if(sibling->left != nullptr && sibling->left->color == 'R')
                    {
                        sibling->left->color = 'B';
                        sibling->color = temp->parent->color;
                        temp->parent->color = 'B';
                        rightRotate(root, temp->parent);
                    }

                    else if(sibling->right != nullptr && sibling->right->color == 'R')
                    {
                        sibling->right->color = 'B';
                        leftRotate(root, sibling);
                        rightRotate(root, temp->parent);
                        sibling->color = temp->parent->color;
                        temp->parent->color = 'B';
                    }

                    // finally delete the node
                    temp->parent->right = nullptr;
                    delete temp;
                    return;
                }

            }

        }

    }

}





int main()
{

return 0;
}
