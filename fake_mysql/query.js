import Parser from "node-sql-parser"
const opt = {
    database: 'MariaDB'
}


const parseQuery = (query) => {
    const parser = new Parser.Parser();
    return parser.parse(query, opt);
}

const max = (a,b) => {return a > b ? a : b}
const min = (a,b) => {return a < b ? a : b}

const MAX_ROWS = 100;

const getLenHelper = (ast) => {
    let currentAST = ast
    let limit = 0;
    if (currentAST === undefined){
        return 0;
    }

    if (currentAST.limit){
        let val =  currentAST.limit.value;
        limit = val[val.length - 1].value
    } else {
        limit = MAX_ROWS;
    }

    if (currentAST["union"] === 'union'){
        limit += getLenHelper(currentAST["_next"]);
    }


    for (const key in currentAST["from"]) {
        let currentEl;
        if (currentAST["from"].hasOwnProperty(key)){
            currentEl = currentAST["from"][key];
        } else continue;
        if (currentEl.hasOwnProperty("expr") && currentEl.hasOwnProperty("join")){
            switch (currentEl["join"]) {
                case "RIGHT JOIN":
                    limit = getLenHelper(currentEl["expr"].ast);
                    break;
                case "JOIN":
                case "INNER JOIN":
                    limit = min(getLenHelper(currentEl["expr"].ast), limit);
                    break;
            }
        }
    }
    console.log(currentAST)
    if (currentAST["_limit"]) {
        let val =  currentAST["_limit"].value;
        limit = min(limit, val[val.length - 1].value);
    }
    return limit;
}

const getLen = ast => getLenHelper(ast.ast)

const toColumnStr = (expr, tables) => {
    if (expr.type === 'column_ref'){
        if (expr.table)
            return [expr.table + '.' + expr.column];
        let result = []
        for (let i = 0; i < tables.length; i++) {
            result.push(tables[i] + '.' + expr.column);
        }
        return result
    }
    return [expr];
}

const appendOrCreate = (field, key, content) => {
    if (!field[key]){
        field[key] = [];
    }
    field[key] = field[key].concat(content)
}

const getFieldMapHelper = (field, ast) => {
    let column = ast.columns;
    let from = ast.from;
    let tables = [];
    for (let i = 0; i < from.length; i++) {
        let currentEl = from[i];
        console.log(currentEl)
        tables = currentEl.as != null ? tables.concat([currentEl.table, currentEl.as]) : tables.concat([currentEl.table]);
    }
    for (let i = 0; i < column.length; i++) {
        let currentEl = column[i];
        console.log(currentEl.expr)
        let res = {
            fields: toColumnStr(currentEl.expr, tables)
        }
        if (currentEl.as){
            res["fields"] = res["fields"].concat( toColumnStr(currentEl.as, tables) )
        }
        field.push(res)
    }
}

const getFieldMap = (ast) => {
    let field = [];
    getFieldMapHelper(field, ast.ast)
    console.log(field)
}

const getCol = (ast) => {
    return ast.columnList.map(v => {
        return v.split("::").splice(1);
    });
}

const getConstraint = (ast) => {
    
}


console.log(getFieldMap(parseQuery("SELECT B.c, C.c AS k from B, C limit 1")))

