using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Data;
using Mono.Data.Sqlite;
using System.IO;

public class DBScript : MonoBehaviour
{
    //store name of database
    private string dbName = "URI=file:robDB.db";
    [System.Serializable]
    public class Position
    {
        public float x;
        public float y;
        public float theta;
        public int time;
    }

    [System.Serializable]
    public class Positions
    {
        public Position[] positions;
    }

    Positions data;
    float currentTime = 0;
    int prevTime = 0;
    int index = -1;

    public TextAsset jsonfile;

    void Start() {
        data = JsonUtility.FromJson<Positions>(jsonfile.text);

        currentTime = data.positions[0].time;
        index = 1;
        
       /* CreateDB();
        string filePath = Path.Combine(Application.streamingAssetsPath, "plugins/data.json");

        if (File.Exists(filePath))
        {
            string json = File.ReadAllText(filePath);
            ParseJSON(json);
            CreateRecord('car1', ParseJSON(json));
        }
        else
        {
            Debug.LogError("File not found: " + filePath);
        }
       
        ReadRecords();*/
    }

    void Update()
    {
        currentTime += Time.deltaTime * 1000;

        if (data.positions.Length - 1 > index && data.positions[index].time  <= currentTime) {
            // Update position

            transform.position = new Vector3(data.positions[index].x, 0, data.positions[index].y);
            transform.eulerAngles = new Vector3(transform.eulerAngles.x, -data.positions[index].theta * (180 / Mathf.PI), transform.eulerAngles.z);

            index++;
        }
    }
    /*
    private MyDataStructure[] ParseJSON(MyDataStructure[] data)
    {
        foreach (MyDataStructure item in data)
        {
            Debug.Log("x: " + item.x);

            Debug.Log("y: " + item.y);
            Debug.Log("theta: " + item.theta);
            Debug.Log("time: " + item.time);
        }
        return data;
    }*/
    /*
    public void CreateDB()
    {
        Debug.Log("Create DB");
        using (var connection = new SqliteConnection(dbName))
        {
            // opens connection
            connection.Open();
            // creates an object named command for controlling the database
            using (var command = connection.CreateCommand())
            {
                // create tabel with SQL syntax for tracking robot motion
                command.CommandText = "CREATE TABLE IF NOT EXISTS robot_movement (name VARCHAR(30)," +
                    "x FLOAT, z FLOAT)";
                // execute command on the database
                command.ExecuteNonQuery();
            }
            // close connection
            connection.Close();
        }
    }


    public void CreateRecord(string name, MyDataStructure[] data)
    {
        Debug.Log("CreateRecord");
        // connect to database
        using (var connection = new SqliteConnection(dbName))
        {
            // open connection
            connection.Open();
            // create object command to control database
            using (var command = connection.CreateCommand())
            {
                foreach (MyDataStructure item in data)
                {
                    // sql to create new robot movement coordinates
                    command.CommandText = "INSERT INTO robot_movement (name, x, z) " +
                        "VALUES ('" + name + "', '" + item.x + "', '" + item.y + "');";
                    // run the query
                    command.ExecuteNonQuery();
                }
            }
            // close connection
            connection.Close();
        }
    }


    public void ReadRecords()
    {
        // connect to database
        using (var connection = new SqliteConnection(dbName))
        {
            // open connection
            connection.Open();
            //create command to allow for database control
            using (var command = connection.CreateCommand())
            {
                //sql statement to read from table 
                command.CommandText = "SELECT * FROM robot_movement;";
                // iterate the returned records
                using (IDataReader reader = command.ExecuteReader())
                {
                    while (reader.Read())
                    {
                        //output the value name, x, z for each record
                        Debug.Log("Name: " + reader["name"] + " X: " + reader["x"] + "Z: " + reader["z"]);
                    }
                    // close reader
                    reader.Close();
                }
            }
            //close connection 
            connection.Close();
        }
    }*/
}
