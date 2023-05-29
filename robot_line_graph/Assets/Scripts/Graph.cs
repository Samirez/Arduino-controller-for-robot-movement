using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(LineRenderer))]

public class Graph : MonoBehaviour
{
    [SerializeField]
    private GameObject pointPrefab;
    private GameObject newPoint;

    private float x;
    private float y;
    private float z;

    [SerializeField]
    private GameObject database;



    // Start is called before the first frame update
    void Awake()
    {
        //Transform point = Instantiate(pointPrefab);
        //point.localPosition = Vector3.right;
        //point = Instantiate(pointPrefab);
        // point.localPosition = Vector3.right * 2f;
        Debug.Log(transform.position.x);
        Debug.Log(transform.position.y);
        Debug.Log(transform.position.z);
        Debug.Log("");

        x = transform.position.x;
        y = transform.position.y;
        z = transform.position.z;


    }

    void Start()
    {
 
    }

 
    // Update is called once per frame
    void Update()
    {/*
        //if x < or > then Instantiate(pointPrefab);
        if (x < (transform.position.x - 1f) || x > (transform.position.x + 1f)) {
            x = transform.position.x;
            
            newPoint = Instantiate(pointPrefab);
            newPoint.transform.position = new Vector3(x, y, z);
            database.GetComponent<DBScript>().CreateRecord(newPoint.name, x, z);
        }

        if (y < (transform.position.y - 1f) || y > (transform.position.y + 1f))
        {
            y = transform.position.y;

            newPoint = Instantiate(pointPrefab);
            newPoint.transform.position = new Vector3(x, y, z);
        }
        if (z < (transform.position.z - 1f) || z > (transform.position.z + 1f))
        {
            z = transform.position.z;

            newPoint = Instantiate(pointPrefab);
            newPoint.transform.position = new Vector3(x, y, z);
            database.GetComponent<DBScript>().CreateRecord(newPoint.name, x, z);
        }*/

    }
}
