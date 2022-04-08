const express = require("express");
const cors = require("cors");
const dotenv = require("dotenv").config();

// Define app and port
const app = express();
const port = process.env.PORT;

// More Middlware
app.use(cors());
app.use(express.json());

app.get("/", async (req, res) => {
    console.log("get ");
    return res.json({ msg: "get request"});
});


app.post("/", async (req, res) => {
    console.log("yo");
    return res.json({ msg: "Success", body: req.body});
});

app.listen(port, () => console.log(`Listening on Port ${port}!`));