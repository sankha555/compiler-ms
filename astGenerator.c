#include "astGenerator.h"
#include "astDef.h"
#include "parserDef.h"
#include "lexerDef.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Creates a new abstract syntax tree node
 * 
 * @param tag The function tag of the node
 * @return astNode* 
 */
astNode *newASTnode(ASTtag tag)
{
    astNode *ptr = (astNode *)malloc(sizeof(astNode));
    ptr->type = tag;
    ptr->leafType = -1;
    ptr->isLeafNode = FALSE;
    ptr->next = NULL;
    ptr->data = NULL;
    ptr->isLinkedListNode = FALSE;
    for (int i = 0; i < MAX_PROD_LEN; i++)
    {
        ptr->children[i] = NULL;
    }
    return ptr;
    ptr->intValue = 0;
    ptr->realValue = 0;
}

/**
 * @brief Creates a leaf node of the abstract syntax tree
 * 
 * @param tag the value (identifier / keyword) of the node
 * @return astNode* 
 */
astNode *newASTleafNode(ASTLeafTag tag)
{
    astNode *ptr = (astNode *)malloc(sizeof(astNode));
    ptr->type = -1;
    ptr->leafType = tag;

    ptr->isLinkedListNode = FALSE;
    ptr->isLeafNode = TRUE;

    ptr->intValue = 0;
    ptr->realValue = 0;

    ptr->next = NULL;
    ptr->data = NULL;
    for (int i = 0; i < MAX_PROD_LEN; i++)
    {
        ptr->children[i] = NULL;
    }

    return ptr;
}

/**
 * @brief Frees the children of the node
 *
 * @param root The node whose children are to be freed
 * @param low (inclusive) The lower bound of the children to be freed
 * @param high (inclusive) The upper bound of the children to be freed
 * @return int 0 on success, -1 on failure
 */
int freeChildren(ParseTreeNode *root, int low, int high)
{
    if ((low > high) || (high >= root->numberOfChildren))
    {
        printf("Error deleting children [%d,%d] of node:\n", low, high);
        printTreeNode(stdout, root);
        return -1;
    }
    for (int i = low; i <= high; i++)
    {
        free(root->children[i]);
    }
    return 0;
}

/**
 * @brief Recursives create a Abstract Syntax Tree from a Parse Tree
 * 
 * @param root The root node of the Parse Tree
 * @return astNode* root of the new Abstract Syntax Tree
 */
astNode *createAbstractSyntaxTree(ParseTreeNode *root)
{

    // generally we have to traverse the child nodes first before
    // applying rules, but not for some cases, in which inhptrs are required

    astNode *ptr = NULL;

    switch (root->ruleIndex + 1)
    {
    case 1:
        /**
         * @brief <program> ===> <otherFunctions> <mainFunction>
         *
         */
        ptr = newASTnode(Program);
        createAbstractSyntaxTree(root->children[0]);
        createAbstractSyntaxTree(root->children[1]);
        ptr->children[0] = root->children[0]->ptr;
        ptr->children[1] = root->children[1]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 1);
        return ptr;
        break;
    case 2:
        /**
         * @brief <mainFunction> ===> TK_MAIN <stmts> TK_END
         *
         */
        ptr = newASTnode(MainFunc);
        createAbstractSyntaxTree(root->children[1]);
        ptr->children[0] = root->children[1]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 2);
        return ptr;
        break;
    case 3:
        /**
         * @brief <otherFunctions> ===> <function> <otherFunctions>
         *
         */
        ptr = newASTnode(FuncLinkedListNode);
        createAbstractSyntaxTree(root->children[0]);
        createAbstractSyntaxTree(root->children[1]);
        ptr->isLinkedListNode = TRUE;
        ptr->data = root->children[0]->ptr;
        ptr->next = root->children[1]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 1);
        return ptr;
        break;
    case 4:
        /**
         * @brief <otherFunctions> ===> epsilon
         *
         */
        root->ptr = NULL;
        freeChildren(root, 0, 0);
        return NULL;
        break;
    case 5:
        /**
         * @brief <function> ===> TK_FUNID <input_par> <output_par> TK_SEM <stmts> TK_END
         *
         */
        ptr = newASTnode(FuncDef);
        createAbstractSyntaxTree(root->children[1]);
        createAbstractSyntaxTree(root->children[2]);
        createAbstractSyntaxTree(root->children[4]);
        ptr->children[0] = newASTleafNode(FuncId);
        ptr->children[0]->entry = root->children[0]->terminal;
        ptr->children[1] = root->children[1]->ptr;
        ptr->children[2] = root->children[2]->ptr;
        ptr->children[4] = root->children[4]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 5);
        return ptr;
        break;
    case 6:
        /**
         * @brief <input_par> ===> TK_INPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR
         *
         */
        createAbstractSyntaxTree(root->children[4]);
        root->ptr = root->children[4]->ptr;
        freeChildren(root, 0, 5);
        return ptr;
        break;
    case 7:
        /**
         * @brief <output_par> ===>TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL  <parameter_list> TK_SQR
         *
         */
        createAbstractSyntaxTree(root->children[4]);
        root->ptr = root->children[4]->ptr;
        freeChildren(root, 0, 5);
        return ptr;
        break;
    case 8:
        /**
         * @brief <output_par> ===> epsilon
         *
         */
        root->ptr = NULL;
        freeChildren(root, 0, 0);
        return NULL;
        break;
    case 9:
        /**
         * @brief <parameter_list> ===> <dataType> TK_ID <remaining_list>
         */
        ;
        createAbstractSyntaxTree(root->children[0]);
        createAbstractSyntaxTree(root->children[2]);
        ptr = newASTnode(ParameterLinkedListNode);
        ptr->isLinkedListNode = TRUE;
        ptr->data = newASTnode(Parameter);
        ptr->data->children[0] = root->children[0]->ptr;
        ptr->data->children[1] = newASTleafNode(VariableId);
        ptr->next = root->children[2]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 2);
        return ptr;
    case 10:
        /**
         * @brief <dataType> ===> <primitiveDatatype>
         */
        createAbstractSyntaxTree(root->children[0]);
        ptr = root->children[0]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 0);
        return ptr;
    case 11:
        /**
         * @brief <dataType> ===> <constructedDatatype>
         */
        createAbstractSyntaxTree(root->children[0]);
        ptr = root->children[0]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 0);
        return ptr;
    case 12:
        /**
         * @brief <primitiveDatatype> ===> TK_INT
         *
         */
        ptr = newASTleafNode(TypeInt);
        root->ptr = ptr;
        freeChildren(root, 0, 0);
        return ptr;
    case 13:
        /**
         * @brief <primitiveDatatype> ===> TK_REAL
         *
         */
        ptr = newASTleafNode(TypeReal);
        root->ptr = ptr;
        freeChildren(root, 0, 0);
        return ptr;
    case 14:
        /**
         * @brief <constructedDatatype> ===> TK_RECORD TK_RUID
         *
         */
        ptr = newASTleafNode(TypeRecord);
        ptr->entry = root->children[1]->terminal;
        root->ptr = ptr;
        freeChildren(root, 0, 1);
        return ptr;
    case 15:
        /**
         * @brief <constructedDatatype> ===> TK_UNION TK_RUID
         *
         */
        ptr = newASTleafNode(TypeUnion);
        ptr->entry = root->children[1]->terminal;
        root->ptr = ptr;
        freeChildren(root, 0, 1);
        return ptr;
    case 16:
        /**
         * @brief <constructedDatatype> ===> TK_RUID
         *
         */
        ptr = newASTleafNode(TypeRecordUnionId);
        ptr->entry = root->children[0]->terminal;
        root->ptr = ptr;
        freeChildren(root, 0, 0);
        return ptr;
    case 17:
        /**
         * @brief <remaining_list> ===> TK_COMMA <parameter_list>
         *
         */
        createAbstractSyntaxTree(root->children[1]);
        ptr = root->children[1]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 1);
        return ptr;
    case 18:
        /**
         * @brief <remaining_list> ===> epsilon
         *
         */
        root->ptr = NULL;
        freeChildren(root, 0, 0);
        return NULL;
    case 19:
        /**
         * @brief <stmts> ===> <typeDefinitions> <declarations> <otherStmts> <returnStmt>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        createAbstractSyntaxTree(root->children[1]);
        createAbstractSyntaxTree(root->children[2]);
        createAbstractSyntaxTree(root->children[3]);
        ptr = newASTnode(Stmts);
        ptr->children[0] = root->children[0]->ptr;
        ptr->children[1] = root->children[1]->ptr;
        ptr->children[2] = root->children[2]->ptr;
        ptr->children[3] = root->children[3]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 4);
        return ptr;
    case 20:
        /**
         * @brief <typeDefinitions> ===> <actualOrRedefined> <typeDefinitions>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        createAbstractSyntaxTree(root->children[1]);
        ptr = newASTnode(TypeDefLinkedListNode);
        ptr->isLinkedListNode = TRUE;
        ptr->data = root->children[0]->ptr;
        ptr->next = root->children[1]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 2);
        return ptr;
    case 21:
        /**
         * @brief <typeDefinitions> ===> epsilon
         *
         */
        root->ptr = NULL;
        freeChildren(root, 0, 0);
        return NULL;
    case 22:
        /**
         * @brief <actualOrRedefined> ===> <typeDefinition>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        ptr = root->children[0]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 0);
        return ptr;
    case 23:
        /**
         * @brief <actualOrRedefined> ===> <definetypestmt>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        ptr = root->children[0]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 0);
        return ptr;
    case 24:
        /**
         * @brief <typeDefinition> ===> TK_RECORD TK_RUID <fieldDefinitions> TK_ENDRECORD
         *
         */
        createAbstractSyntaxTree(root->children[2]);
        ptr = newASTnode(TypeRecordDefinition);
        ptr->children[0] = newASTleafNode(TypeRecord);
        ptr->children[0]->entry = root->children[1]->terminal;
        ptr->children[1] = root->children[2]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 3);
        return ptr;
    case 25:
        /**
         * @brief <typeDefinition> ===> TK_UNION TK_RUID <fieldDefinitions> TK_ENDUNION
         *
         */
        createAbstractSyntaxTree(root->children[2]);
        ptr = newASTnode(TypeUnionDefinition);
        ptr->children[0] = newASTleafNode(TypeUnion);
        ptr->children[0]->entry = root->children[1]->terminal;
        ptr->children[1] = root->children[2]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 3);
        return ptr;
    case 26:
        /**
         * @brief <fieldDefinitions> ===> <fieldDefinition>1 <fieldDefinition>2 <moreFields>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        createAbstractSyntaxTree(root->children[1]);
        createAbstractSyntaxTree(root->children[2]);
        ptr = newASTnode(FieldDefLinkedListNode);
        ptr->isLinkedListNode = TRUE;
        ptr->data = root->children[0]->ptr;
        ptr->next = newASTnode(FieldDefLinkedListNode);
        ptr->next->isLinkedListNode = TRUE;
        ptr->next->data = root->children[1]->ptr;
        ptr->next->next = root->children[2]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 3);
        return ptr;
    case 27:
        /**
         * @brief <fieldDefinition> ===> TK_TYPE <fieldType> TK_COLON TK_FIELDID TK_SEM
         *
         */
        createAbstractSyntaxTree(root->children[1]);
        ptr = newASTnode(FieldDefinition);
        ptr->children[0] = root->children[1]->ptr;
        ptr->children[1] = newASTleafNode(FieldId);
        ptr->children[1]->entry = root->children[3]->terminal;
        root->ptr = ptr;
        freeChildren(root, 0, 4);
        return ptr;
    case 28:
        /**
         * @brief <fieldType> ===> <primitiveDatatype>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        ptr = root->children[0]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 0);
        return ptr;
    case 29:
        /**
         * @brief <fieldType> ===> TK_RUID
         *
         */
        ptr = newASTleafNode(FieldTypeRUID);
        ptr->entry = root->children[0]->terminal;
        root->ptr = ptr;
        freeChildren(root, 0, 0);
        return ptr;
    case 30:
        /**
         * @brief <moreFields> ===> <fieldDefinition> <moreFields>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        createAbstractSyntaxTree(root->children[1]);
        ptr = newASTnode(FieldDefLinkedListNode);
        ptr->isLinkedListNode = TRUE;
        ptr->data = root->children[0]->ptr;
        ptr->next = root->children[1]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 2);
        return ptr;
    case 31:
        /**
         * @brief <moreFields> ===> epsilon
         *
         */
        root->ptr = NULL;
        freeChildren(root, 0, 0);
        return NULL;
    case 32:
        /**
         * @brief <declarations> ===> <declaration> <declarations>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        createAbstractSyntaxTree(root->children[1]);
        ptr = newASTnode(DeclarationLinkedListNode);
        ptr->isLinkedListNode = TRUE;
        ptr->data = root->children[0]->ptr;
        ptr->next = root->children[1]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 2);
        return ptr;
    case 33:
        /**
         * @brief <declarations> ===> epsilon
         *
         */
        root->ptr = NULL;
        freeChildren(root, 0, 0);
        return NULL;
    case 34:
        /**
         * @brief <declaration> ===> TK_TYPE <dataType> TK_COLON TK_ID <global_or_not> TK_SEM
         *
         */
        createAbstractSyntaxTree(root->children[1]);
        createAbstractSyntaxTree(root->children[4]);
        ptr = newASTnode(Declaration);
        ptr->children[0] = root->children[1]->ptr;
        ptr->children[1] = newASTleafNode(VariableId);
        ptr->children[1]->entry = root->children[3]->terminal;
        ptr->children[2] = root->children[4]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 5);
        return ptr;
    case 35:
        /**
         * @brief <global_or_not> ===> TK_COLON TK_GLOBAL
         *
         */
        ptr = newASTleafNode(Global);
        root->ptr = ptr;
        freeChildren(root, 0, 1);
        return ptr;
    case 36:
        /**
         * @brief <global_or_not> ===> epsilon
         *
         */
        root->ptr = NULL;
        freeChildren(root, 0, 0);
        return NULL;
    case 37:
        /**
         * @brief <otherStmts> ===> <stmt> <otherStmts>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        createAbstractSyntaxTree(root->children[1]);
        ptr = newASTnode(StmtLinkedListNode);
        ptr->isLinkedListNode = TRUE;
        ptr->data = root->children[0]->ptr;
        ptr->next = root->children[1]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 2);
        return ptr;
    case 38:
        /**
         * @brief <otherStmts> ===> epsilon
         *
         */
        root->ptr = NULL;
        freeChildren(root, 0, 0);
        return NULL;
    case 39:
        /**
         * @brief <stmt> ===> <assignmentStmt>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        ptr = root->children[0]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 0);
        return ptr;
    case 40:
        /**
         * @brief <stmt> ===> <iterativeStmt>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        ptr = root->children[0]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 0);
        return ptr;
    case 41:
        /**
         * @brief <stmt> ===> <conditionalStmt>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        ptr = root->children[0]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 0);
        return ptr;
    case 42:
        /**
         * @brief <stmt> ===> <ioStmt>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        ptr = root->children[0]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 0);
        return ptr;
    case 43:
        /**
         * @brief <stmt> ===> <funCallStmt>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        ptr = root->children[0]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 0);
        return ptr;
    case 44:
        /**
         * @brief <assignmentStmt> ===> <singleOrRecId> TK_ASSIGNOP <arithmeticExpression> TK_SEM
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        createAbstractSyntaxTree(root->children[2]);
        ptr = newASTnode(AssignmentOperation);
        ptr->children[0] = root->children[0]->ptr;
        ptr->children[1] = root->children[2]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 3);
        return ptr;
    case 45:
        /**
         * @brief <singleOrRecId> ===> TK_ID <optionSingleConstructed>
         *
         */
        createAbstractSyntaxTree(root->children[1]);
        ptr = newASTnode(SingleOrRecIdLinkedListNode);
        ptr->isLinkedListNode = TRUE;
        ptr->data = newASTleafNode(VariableId);
        ptr->data->entry = root->children[0]->terminal;
        ptr->next = root->children[1]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 1);
        return ptr;
    case 46:
        /**
         * @brief <optionSingleConstructed> ===> <oneExpansion> <moreExpansions>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        createAbstractSyntaxTree(root->children[1]);
        ptr = newASTnode(FieldIdLinkedListNode);
        ptr->isLinkedListNode = TRUE;
        ptr->data = root->children[0]->ptr;
        ptr->next = root->children[1]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 1);
        return ptr;
    case 47:
        /**
         * @brief <optionSingleConstructed> ===> epsilon
         *
         */
        root->ptr = NULL;
        freeChildren(root, 0, 0);
        return NULL;
    case 48:
        /**
         * @brief <oneExpansion> ===> TK_DOT TK_FIELDID
         *
         */
        ptr = newASTleafNode(FieldId);
        ptr->entry = root->children[1]->terminal;
        root->ptr = ptr;
        freeChildren(root, 0, 2);
        return ptr;
    case 49:
        /**
         * @brief <moreExpansions> ===> <oneExpansion> <moreExpansions>
         *
         */
        createAbstractSyntaxTree(root->children[0]);
        createAbstractSyntaxTree(root->children[1]);
        ptr = newASTnode(FieldIdLinkedListNode);
        ptr->isLinkedListNode = TRUE;
        ptr->data = root->children[0]->ptr;
        ptr->next = root->children[1]->ptr;
        root->ptr = ptr;
        freeChildren(root, 0, 1);
        return ptr;
    case 50:
        /**
         * @brief <moreExpansions> ===> epsilon
         *
         */
        root->ptr = NULL;
        freeChildren(root, 0, 0);
        return NULL;

	case 76:
		/* <var> ===> <singleOrRecId> */
		createAbtstractSyntaxTree(root->children[0]);
		ptr = root->children[0]->ptr;
		root->ptr = ptr;
		freeChildren(root, 0, 0);
		return ptr;

	case 77:
		/* <var> ===> TK_NUM */
		ptr = newASTleafNode(Num);
		ptr->entry = root->children[0]->terminal;
		root->ptr = ptr;
		freeChildren(root, 0, 0);
		return ptr;

	case 78:
		/* <var> ===> TK_RNUM */
		ptr = newASTleafNode(RealNum);
		ptr->entry = root->children[0]->terminal;
		root->ptr = ptr;
		freeChildren(root, 0, 0);
		return ptr;

	case 79:
		/* <logicalOp> ===> TK_AND */
		ptr = newASTleafNode(logOp_AND);
		root->ptr = ptr;
		freeChildren(root, 0, 0);
		return ptr;

	case 80:
		/* <logicalOp> ===> TK_OR */
		ptr = newASTleafNode(logOp_OR);
		root->ptr = ptr;
		freeChildren(root, 0, 0);
		return ptr;

	case 81:
		/* <relationalOp> ===> TK_LT */
		ptr = newASTleafNode(relOp_LT);
		root->ptr = ptr;
		freeChildren(root, 0, 0);
		return ptr;


	case 82:
		/* <relationalOp> ===> TK_LE */
		ptr = newASTleafNode(relOp_LE);
		root->ptr = ptr;
		freeChildren(root, 0, 0);
		return ptr;


	case 83:
		/* <relationalOp> ===> TK_EQ */
		ptr = newASTleafNode(relOp_EQ);
		root->ptr = ptr;
		freeChildren(root, 0, 0);
		return ptr;


	case 84:
		/* <relationalOp> ===> TK_GT */
		ptr = newASTleafNode(relOp_GT);
		root->ptr = ptr;
		freeChildren(root, 0, 0);
		return ptr;

	case 85:
		/* <relationalOp> ===> TK_GE */
		ptr = newASTleafNode(relOp_GE);
		root->ptr = ptr;
		freeChildren(root, 0, 0);
		return ptr;

	case 86:
		/* <relationalOp> ===> TK_NE */
		ptr = newASTleafNode(relOp_NE);
		root->ptr = ptr;
		freeChildren(root, 0, 0);
		return ptr;

	case 87:
		/* <returnStmt> ===> TK_RETURN <optionalReturn> TK_SEM */
		createAbstractSyntaxTree(root->children[1]);
		ptr = newASTnode(Return);
		ptr->children[0] = root->children[1]->ptr;
		root->ptr = ptr;
		freeChildren(root, 0, 2);
		return ptr;

	case 88:
		/* <optionalReturn> ===> TK_SQL <idList> TK_SQR */
		createAbstractSyntaxTree(root->children[1]);
		ptr = root->children[1]->ptr;
		root->ptr = ptr;
		freeChildren(root, 0, 2);
		return ptr;

	case 89:
		/* <optionalReturn> ===> epsilon */
		root->ptr = NULL;
		freeChildren(root, 0, 0);
		return NULL;

	case 90:
		/* <idList> ===> TK_ID <more_ids> */
		createAbstractSyntaxTree(root->children[1]);
		ptr = newASTnode(IdLinkedListNode);
		ptr->isLinkedListNode = TRUE;
		ptr->data = newASTleafNode(Id);
		ptr->data->entry = root->children[0]->terminal;
		ptr->next = root->children[1]->ptr;
		root->ptr = ptr;
		freeChildren(root, 0, 1);
		return ptr;

	case 91:
		/* <more_ids> ===> TK_COMMA <idList> */
		createAbstractSyntaxTree(root->children[1]);
		ptr = root->children[1]->ptr;
		root->ptr = ptr;
		freeChildren(root, 0, 1);
		return ptr;

	case 92:
		/* <more_ids> ===> epsilon */
		root->ptr = NULL;
		freeChildren(root, 0, 0);
		return NULL;

	case 93:
		/* <definetypestmt> ===> TK_DEFINETYPE <A> TK_RUID TK_AS TK_RUID */
		ptr = newASTNode(DefineType);
		createAbstractSyntaxTree(root->children[1]);
		ptr->children[0] = root->children[1]->ptr;
		ptr->children[1] = newASTleafNode(RecUnionId);
		ptr->children[1]->entry = root->children[2]->terminal;
		ptr->children[2] = newASTleafNode(RecUnionId);
		ptr->children[2]->entry = root->children[4]->terminal;
		root->ptr = ptr;
		freeChildren(root, 0, 4);
		return ptr;

	case 94:
		/* <A> ===> TK_RECORD */
		ptr = newASTleafNode(Record);
		root->ptr = ptr;
		freeChildren(root, 0, 0);
		return ptr;

	case 95:
		/* <A> ===> TK_UNION */
		ptr = newASTleafNode(Union);
		root->ptr = ptr;
		freeChildren(root, 0, 0);
		return ptr;

    default:
        printf("Unmatched rule encountered. Exiting!!\n");
        exit(1);
        break;
    }

    return NULL;
}
